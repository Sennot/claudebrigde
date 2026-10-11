#include <winsock2.h>
#include <ws2tcpip.h>

#include "http_server.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string_view>
#include <thread>

namespace bridge::http {
    namespace {
        constexpr size_t kMaxHeaderBytes = 64 * 1024;
        constexpr size_t kMaxBodyBytes = 64 * 1024 * 1024;

        std::string lower(std::string s) {
            std::ranges::transform(s, s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return s;
        }

        std::string trim(std::string_view s) {
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.remove_prefix(1);
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.remove_suffix(1);
            return std::string(s);
        }

        int hexValue(char c) {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        std::string urlDecode(std::string_view s) {
            std::string out;
            out.reserve(s.size());
            for (size_t i = 0; i < s.size(); i++) {
                if (s[i] == '+') {
                    out += ' ';
                } else if (s[i] == '%' && i + 2 < s.size() && hexValue(s[i + 1]) >= 0 && hexValue(s[i + 2]) >= 0) {
                    out += static_cast<char>(hexValue(s[i + 1]) * 16 + hexValue(s[i + 2]));
                    i += 2;
                } else {
                    out += s[i];
                }
            }
            return out;
        }

        void parseTarget(std::string_view target, Request& req) {
            auto q = target.find('?');
            req.path = urlDecode(target.substr(0, q));
            if (q == std::string_view::npos) return;
            auto rest = target.substr(q + 1);
            while (!rest.empty()) {
                auto amp = rest.find('&');
                auto pair = rest.substr(0, amp);
                auto eq = pair.find('=');
                if (eq == std::string_view::npos) {
                    req.query[urlDecode(pair)] = "";
                } else {
                    req.query[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
                }
                if (amp == std::string_view::npos) break;
                rest.remove_prefix(amp + 1);
            }
        }

        bool readRequest(SOCKET client, Request& req) {
            std::string data;
            char buffer[16 * 1024];
            size_t headerEnd;
            while ((headerEnd = data.find("\r\n\r\n")) == std::string::npos) {
                int n = recv(client, buffer, sizeof(buffer), 0);
                if (n <= 0 || data.size() > kMaxHeaderBytes) return false;
                data.append(buffer, n);
            }

            std::string_view head(data.data(), headerEnd);
            auto lineEnd = head.find("\r\n");
            auto requestLine = head.substr(0, lineEnd);
            auto sp1 = requestLine.find(' ');
            auto sp2 = requestLine.find(' ', sp1 + 1);
            if (sp1 == std::string_view::npos || sp2 == std::string_view::npos) return false;
            req.method = std::string(requestLine.substr(0, sp1));
            parseTarget(requestLine.substr(sp1 + 1, sp2 - sp1 - 1), req);

            while (lineEnd != std::string_view::npos) {
                head.remove_prefix(lineEnd + 2);
                lineEnd = head.find("\r\n");
                auto line = head.substr(0, lineEnd);
                auto colon = line.find(':');
                if (colon == std::string_view::npos) continue;
                req.headers[lower(trim(line.substr(0, colon)))] = trim(line.substr(colon + 1));
            }

            size_t length = 0;
            if (auto it = req.headers.find("content-length"); it != req.headers.end()) {
                length = std::strtoull(it->second.c_str(), nullptr, 10);
                if (length > kMaxBodyBytes) return false;
            }

            req.body = data.substr(headerEnd + 4);
            while (req.body.size() < length) {
                int n = recv(client, buffer, sizeof(buffer), 0);
                if (n <= 0) return false;
                req.body.append(buffer, n);
            }
            req.body.resize(length);
            return true;
        }

        char const* reason(int status) {
            switch (status) {
                case 200: return "OK";
                case 400: return "Bad Request";
                case 403: return "Forbidden";
                case 404: return "Not Found";
                case 409: return "Conflict";
                case 500: return "Internal Server Error";
                case 504: return "Gateway Timeout";
                default: return "Unknown";
            }
        }

        void sendAll(SOCKET client, std::string_view data) {
            while (!data.empty()) {
                int n = send(client, data.data(), static_cast<int>(std::min<size_t>(data.size(), 1 << 20)), 0);
                if (n <= 0) return;
                data.remove_prefix(n);
            }
        }

        void writeResponse(SOCKET client, Response const& res) {
            std::string head = "HTTP/1.1 " + std::to_string(res.status) + " " + reason(res.status) + "\r\n";
            head += "Content-Type: " + res.contentType + "; charset=utf-8\r\n";
            head += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
            head += "Connection: close\r\n\r\n";
            sendAll(client, head);
            sendAll(client, res.body);
        }

        // Web pages can reach 127.0.0.1 too. Requests from a browser carry an
        // Origin header, and a custom header cannot be sent cross-origin
        // without a CORS preflight this server never approves.
        bool fromLocalTool(Request const& req) {
            return !req.headers.contains("origin") && req.headers.contains("x-gd-bridge");
        }

        void serve(SOCKET listener, Handler handler) {
            while (true) {
                SOCKET client = accept(listener, nullptr, nullptr);
                if (client == INVALID_SOCKET) continue;

                DWORD timeout = 10000;
                setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char const*>(&timeout), sizeof(timeout));

                Request req;
                if (readRequest(client, req)) {
                    Response res;
                    if (!fromLocalTool(req)) {
                        res = {403, "application/json", R"({"ok":false,"error":"send header X-GD-Bridge: 1"})"};
                    } else {
                        try {
                            res = handler(req);
                        } catch (...) {
                            res = {500, "application/json", R"({"ok":false,"error":"handler failed"})"};
                        }
                    }
                    writeResponse(client, res);
                }
                shutdown(client, SD_SEND);
                closesocket(client);
            }
        }
    }

    std::string start(unsigned short port, Handler handler) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return "WSAStartup failed";

        SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listener == INVALID_SOCKET) return "socket() failed: " + std::to_string(WSAGetLastError());

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            auto err = WSAGetLastError();
            closesocket(listener);
            return "bind() on port " + std::to_string(port) + " failed: " + std::to_string(err);
        }
        if (listen(listener, 8) == SOCKET_ERROR) {
            auto err = WSAGetLastError();
            closesocket(listener);
            return "listen() failed: " + std::to_string(err);
        }

        // Lives until the process exits.
        std::thread(serve, listener, std::move(handler)).detach();
        return {};
    }
}
