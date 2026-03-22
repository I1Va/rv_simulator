#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace rv
{

const static inline std::vector<std::string> reg_names = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

enum Reg : uint8_t {
    x0 = 0,   x1 = 1,   x2 = 2,   x3 = 3,   x4 = 4,   x5 = 5,   x6 = 6,   x7 = 7,
    x8 = 8,   x9 = 9,   x10 = 10, x11 = 11, x12 = 12, x13 = 13, x14 = 14, x15 = 15,
    x16 = 16, x17 = 17, x18 = 18, x19 = 19, x20 = 20, x21 = 21, x22 = 22, x23 = 23,
    x24 = 24, x25 = 25, x26 = 26, x27 = 27, x28 = 28, x29 = 29, x30 = 30, x31 = 31,

    zero = x0, ra = x1,  sp = x2,  gp = x3,  tp = x4,  t0 = x5,  t1 = x6,  t2 = x7,
    s0   = x8, s1 = x9,  a0 = x10, a1 = x11, a2 = x12, a3 = x13, a4 = x14, a5 = x15,
    a6   = x16, a7 = x17, s2 = x18, s3 = x19, s4 = x20, s5 = x21, s6 = x22, s7 = x23,
    s8   = x24, s9 = x25, s10 = x26, s11 = x27, t3 = x28, t4 = x29, t5 = x30, t6 = x31,

    fp = s0
};

const static inline uint32_t PAGE_SIZE = 0x1000;

struct Config {
    std::string isa = "rv32i"; 
    std::string elf_path = "";
    std::string init_state_path = "";
    std::string final_state_path = "";
    bool interactive = false;
    bool logs_disabled = false;
    int steps = 10;
};

} // namespace rv