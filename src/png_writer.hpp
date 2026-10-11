#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace bridge::png {
    // Writes 8-bit RGB pixels (top row first) as an uncompressed PNG.
    // Screenshots are small, so skipping compression keeps this dependency-free.
    bool writeRGB(std::string const& path, std::vector<uint8_t> const& rgb, int width, int height);
}
