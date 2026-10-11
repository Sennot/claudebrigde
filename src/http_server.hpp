#pragma once

#include <functional>
#include <map>
#include <string>

// Minimal blocking HTTP/1.1 server on 127.0.0.1. Requests are handled one at a
// time on a background thread; the handler is called on that thread.
namespace bridge::http {
    struct Request {
        std::string method;
        std::string path;
        std::map<std::string, std::string> query;
        std::map<std::string, std::string> headers; // keys lowercased
        std::string body;
    };

    struct Response {
        int status = 200;
        std::string contentType = "application/json";
        std::string body;
    };

    using Handler = std::function<Response(Request const&)>;

    // Returns an empty string on success, otherwise the error.
    std::string start(unsigned short port, Handler handler);
}
