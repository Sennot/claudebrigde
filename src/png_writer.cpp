#include "png_writer.hpp"

#include <algorithm>
#include <array>
#include <fstream>

namespace bridge::png {
    namespace {
        std::array<uint32_t, 256> const& crcTable() {
            static auto const table = [] {
                std::array<uint32_t, 256> t{};
                for (uint32_t n = 0; n < 256; n++) {
                    uint32_t c = n;
                    for (int k = 0; k < 8; k++) c = (c & 1) ? 0xedb88320u ^ (c >> 1) : c >> 1;
                    t[n] = c;
                }
                return t;
            }();
            return table;
        }

        uint32_t crc(uint8_t const* data, size_t len, uint32_t c = 0xffffffffu) {
            auto const& t = crcTable();
            for (size_t i = 0; i < len; i++) c = t[(c ^ data[i]) & 0xff] ^ (c >> 8);
            return c;
        }

        void put32(std::vector<uint8_t>& out, uint32_t v) {
            out.push_back(static_cast<uint8_t>(v >> 24));
            out.push_back(static_cast<uint8_t>(v >> 16));
            out.push_back(static_cast<uint8_t>(v >> 8));
            out.push_back(static_cast<uint8_t>(v));
        }

        void chunk(std::vector<uint8_t>& out, char const* type, std::vector<uint8_t> const& data) {
            put32(out, static_cast<uint32_t>(data.size()));
            size_t start = out.size();
            out.insert(out.end(), type, type + 4);
            out.insert(out.end(), data.begin(), data.end());
            put32(out, crc(out.data() + start, out.size() - start) ^ 0xffffffffu);
        }
    }

    bool writeRGB(std::string const& path, std::vector<uint8_t> const& rgb, int width, int height) {
        // Raw scanlines, each prefixed with filter type 0.
        std::vector<uint8_t> raw;
        raw.reserve(static_cast<size_t>(height) * (width * 3 + 1));
        for (int y = 0; y < height; y++) {
            raw.push_back(0);
            auto row = rgb.begin() + static_cast<ptrdiff_t>(y) * width * 3;
            raw.insert(raw.end(), row, row + width * 3);
        }

        // zlib stream made of stored deflate blocks.
        std::vector<uint8_t> z{0x78, 0x01};
        uint32_t a = 1, b = 0;
        for (uint8_t v : raw) {
            a = (a + v) % 65521;
            b = (b + a) % 65521;
        }
        size_t pos = 0;
        do {
            size_t len = std::min<size_t>(65535, raw.size() - pos);
            bool last = pos + len == raw.size();
            z.push_back(last ? 1 : 0);
            z.push_back(static_cast<uint8_t>(len));
            z.push_back(static_cast<uint8_t>(len >> 8));
            z.push_back(static_cast<uint8_t>(~len));
            z.push_back(static_cast<uint8_t>(~len >> 8));
            z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + len);
            pos += len;
        } while (pos < raw.size());
        put32(z, (b << 16) | a);

        std::vector<uint8_t> header;
        put32(header, static_cast<uint32_t>(width));
        put32(header, static_cast<uint32_t>(height));
        header.insert(header.end(), {8, 2, 0, 0, 0}); // 8-bit RGB

        std::vector<uint8_t> file{0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
        chunk(file, "IHDR", header);
        chunk(file, "IDAT", z);
        chunk(file, "IEND", {});

        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<char const*>(file.data()), static_cast<std::streamsize>(file.size()));
        return static_cast<bool>(out);
    }
}
