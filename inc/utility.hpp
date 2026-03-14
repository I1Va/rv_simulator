#pragma once

#include <format>
#include <string>
#include <cstdint>

inline std::string to_hex(uint32_t val) {
    return std::format("0x{:08x}", val);
}
