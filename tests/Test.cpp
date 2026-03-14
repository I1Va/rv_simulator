#include <gtest/gtest.h>

#include "simulator.hpp"

static const char isa_str[] = "rv32i";

static const rv::Parser::SegmentInfo DEFAULT_TEXT_SEGMENT = 
{
    .vaddr = 0,
    .memsz = 128,
    .filesz = 128,
    .data = std::vector<uint8_t>(128),
    .r = true,
    .w = false,
    .x = true
};

static const rv::Parser::SegmentInfo DEFAULT_DATA_SEGMENT = 
{
    .vaddr = 256,
    .memsz = 128,
    .filesz = 128,
    .data = std::vector<uint8_t>(256),
    .r = true,
    .w = true,
    .x = false
};

#define X0  0  // zero
#define X1  1  // ra (return address)
#define X2  2  // sp (stack pointer)
#define X3  3  // gp (global pointer)
#define X4  4  // tp (thread pointer)
#define X5  5  // t0 (temporary)
#define X6  6  // t1
#define X7  7  // t2
#define X8  8  // s0/fp (saved register / frame pointer)
#define X9  9  // s1
#define X10 10 // a0 (argument / return value)
#define X11 11 // a1
#define X12 12 // a2
#define X13 13 // a3
#define X14 14 // a4
#define X15 15 // a5
#define X16 16 // a6
#define X17 17 // a7
#define X18 18 // s2 (saved registers)
#define X19 19 // s3
#define X20 20 // s4
#define X21 21 // s5
#define X22 22 // s6
#define X23 23 // s7
#define X24 24 // s8
#define X25 25 // s9
#define X26 26 // s10
#define X27 27 // s11
#define X28 28 // t3 (temporaries)
#define X29 29 // t4
#define X30 30 // t5
#define X31 31 // t6

rv::Simulator create_sim() {
    rv::Simulator sim(isa_str);
    sim.add_segment(DEFAULT_TEXT_SEGMENT);
    sim.add_segment(DEFAULT_DATA_SEGMENT);
    return sim;
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


// --- R-Type Arithmetic Tests ---

TEST(Arithmetic, ADD) {
    auto sim = create_sim();
    sim.write_reg(X5, 100);
    sim.write_reg(X6, 50);
    
    sim.execute_instr(rv::Instruction(rv::ADD{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 150);
}

TEST(Arithmetic, SUB) {
    auto sim = create_sim();
    sim.write_reg(X5, 100);
    sim.write_reg(X6, 30);
    
    sim.execute_instr(rv::Instruction(rv::SUB{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 70);
}

TEST(Arithmetic, SLT_Signed) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // -1
    sim.write_reg(X6, 1);          //  1
    
    sim.execute_instr(rv::Instruction(rv::SLT{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 1); // -1 < 1 is true
}

TEST(Arithmetic, SLTU_Unsigned) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // Large unsigned
    sim.write_reg(X6, 1);
    
    sim.execute_instr(rv::Instruction(rv::SLTU{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 0); // Max unsigned is not < 1
}

TEST(Arithmetic, Bitwise) {
    auto sim = create_sim();
    sim.write_reg(X5, 0b1010);
    sim.write_reg(X6, 0b1100);
    
    sim.execute_instr(rv::Instruction(rv::AND{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 0b1000);

    sim.execute_instr(rv::Instruction(rv::OR{X8, X5, X6}));
    EXPECT_EQ(sim.read_reg(X8), 0b1110);

    sim.execute_instr(rv::Instruction(rv::XOR{X9, X5, X6}));
    EXPECT_EQ(sim.read_reg(X9), 0b0110);
}

TEST(Arithmetic, Shifts) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xF0000000);
    sim.write_reg(X6, 4);
    
    // Logic Right
    sim.execute_instr(rv::Instruction(rv::SRL{X7, X5, X6}));
    EXPECT_EQ(sim.read_reg(X7), 0x0F000000);

    // Arithmetic Right (Sign extension)
    sim.execute_instr(rv::Instruction(rv::SRA{X8, X5, X6}));
    EXPECT_EQ(sim.read_reg(X8), 0xFF000000);
    
    // Left
    sim.write_reg(X5, 0x1);
    sim.execute_instr(rv::Instruction(rv::SLL{X9, X5, X6}));
    EXPECT_EQ(sim.read_reg(X9), 0x10);
}

// --- I-Type Arithmetic Tests (Immediates) ---

TEST(ArithmeticImm, ADDI) {
    auto sim = create_sim();
    sim.write_reg(X5, 10);
    
    sim.execute_instr(rv::Instruction(rv::ADDI{X6, X5, -15}));
    EXPECT_EQ(sim.read_reg(X6), (uint32_t)-5);
}

TEST(ArithmeticImm, SLTI_SLTIU) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // -1
    
    // Signed: -1 < 0
    sim.execute_instr(rv::Instruction(rv::SLTI{X6, X5, 0}));
    EXPECT_EQ(sim.read_reg(X6), 1);

    // Unsigned: MaxInt < 2047
    sim.execute_instr(rv::Instruction(rv::SLTIU{X7, X5, 2047}));
    EXPECT_EQ(sim.read_reg(X7), 0);
}

TEST(ArithmeticImm, XORI_ORI_ANDI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x0FF);
    
    sim.execute_instr(rv::Instruction(rv::ANDI{X6, X5, 0x00F}));
    EXPECT_EQ(sim.read_reg(X6), 0x00F);

    sim.execute_instr(rv::Instruction(rv::ORI{X7, X5, 0xF00}));
    EXPECT_EQ(sim.read_reg(X7), 0xFFF);
}

TEST(ArithmeticImm, SLLI_SRLI_SRAI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x80000000);
    
    // Shift Right Arithmetic Immediate
    sim.execute_instr(rv::Instruction(rv::SRAI{X6, X5, 1}));
    EXPECT_EQ(sim.read_reg(X6), 0xC0000000);
    
    // Shift Left Logic Immediate
    sim.write_reg(X10, 0x1);
    sim.execute_instr(rv::Instruction(rv::SLLI{X11, X10, 5}));
    EXPECT_EQ(sim.read_reg(X11), 32);
}

TEST(Arithmetic, ZeroRegisterProtection) {
    auto sim = create_sim();
    sim.write_reg(X1, 100);
    sim.write_reg(X2, 100);
    
    // Attempt to write to X0
    sim.execute_instr(rv::Instruction(rv::ADD{X0, X1, X2}));
    
    // X0 must remain 0
    EXPECT_EQ(sim.read_reg(X0), 0);
}

TEST(Arithmetic, XORI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xAA); // 010101010
    
    // 0xAA XOR 0xFF = 0x55
    sim.execute_instr(rv::Instruction(rv::XORI{X6, X5, 0xFF}));
    EXPECT_EQ(sim.read_reg(X6), 0x55);
}

TEST(Arithmetic, SRLI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x80000000); // Only MSB set
    
    // Logical shift right should insert 0s, not preserve the sign
    sim.execute_instr(rv::Instruction(rv::SRLI{X6, X5, 1}));
    EXPECT_EQ(sim.read_reg(X6), 0x40000000);
}

// --- UpperImmediate ---
TEST(UpperImmediate, LUI) {
    auto sim = create_sim();
    
    // LUI loads the upper 20 bits. 0x12345 becomes 0x12345000
    sim.execute_instr(rv::Instruction(rv::LUI{X10, 0x12345}));
    EXPECT_EQ(sim.read_reg(X10), 0x12345000);
}

TEST(UpperImmediate, AUIPC) {
    auto sim = create_sim();
    sim.set_pc(0x1000);
    
    // Current PC (0x1000) + 0x2000 << 12
    sim.execute_instr(rv::Instruction(rv::AUIPC{X10, 0x2})); 
    EXPECT_EQ(sim.read_reg(X10), 0x1000 + (0x2 << 12));
}


// --- Loads & Stores Tests---
TEST(Memory, LoadStoreByte) {
    auto sim = create_sim();
    uint32_t addr = 256; // Data segment start
    sim.write_reg(X5, 0xABCDEF88);
    
    // Store Byte
    sim.execute_instr(rv::Instruction(rv::SB{X5, X0, (int32_t)addr}));
    
    // Load Byte (Signed - should sign extend 0x88 to 0xFFFFFF88)
    sim.execute_instr(rv::Instruction(rv::LB{X6, X0, (int32_t)addr}));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFFFF88);

    // Load Byte Unsigned (Zero extend)
    sim.execute_instr(rv::Instruction(rv::LBU{X7, X0, (int32_t)addr}));
    EXPECT_EQ(sim.read_reg(X7), 0x00000088);
}

TEST(Memory, LoadStoreWord) {
    auto sim = create_sim();
    uint32_t addr = 260;
    sim.write_reg(X10, 0xDEADBEEF);

    sim.execute_instr(rv::Instruction(rv::SW{X10, X0, (int32_t)addr}));
    sim.execute_instr(rv::Instruction(rv::LW{X11, X0, (int32_t)addr}));
    
    EXPECT_EQ(sim.read_reg(X11), 0xDEADBEEF);
}

TEST(Memory, HalfWordLoadStore) {
    auto sim = create_sim();
    uint32_t addr = 260;
    sim.write_reg(X5, 0x00008001); // 0x8001 has bit 15 set

    // Store Half-word
    sim.execute_instr(rv::Instruction(rv::SH{X5, X0, (int32_t)addr}));

    // LH: Signed load should sign-extend bit 15 to bits 16-31
    sim.execute_instr(rv::Instruction(rv::LH{X6, X0, (int32_t)addr}));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFF8001);

    // LHU: Unsigned load should zero-extend
    sim.execute_instr(rv::Instruction(rv::LHU{X7, X0, (int32_t)addr}));
    EXPECT_EQ(sim.read_reg(X7), 0x00008001);
}

// --- Branch Tests---

TEST(Branch, ComparisonLogic) {
    auto sim = create_sim();
    sim.set_pc(0x100);
    sim.write_reg(X5, 10);
    sim.write_reg(X6, 20);

    // BEQ: 10 == 20 (False) -> PC should be PC + 4
    sim.execute_instr(rv::Instruction(rv::BEQ{X5, X6, 100}));
    EXPECT_EQ(sim.pc(), 0x104);

    // BNE: 10 != 20 (True) -> PC should be PC + 100
    sim.set_pc(0x100);
    sim.execute_instr(rv::Instruction(rv::BNE{X5, X6, 100}));
    EXPECT_EQ(sim.pc(), 0x164);

    // BLT: 10 < 20 (True) -> PC jump
    sim.set_pc(0x100);
    sim.execute_instr(rv::Instruction(rv::BLT{X5, X6, 100}));
    EXPECT_EQ(sim.pc(), 0x164);
}

TEST(Branch, BGE_Signed) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // -1
    sim.write_reg(X6, 1);          //  1
    sim.set_pc(0x100);

    // -1 >= 1 is False
    sim.execute_instr(rv::Instruction(rv::BGE{X5, X6, 0x20}));
    EXPECT_EQ(sim.pc(), 0x104);

    // 1 >= -1 is True
    sim.set_pc(0x100);
    sim.execute_instr(rv::Instruction(rv::BGE{X6, X5, 0x20}));
    EXPECT_EQ(sim.pc(), 0x120);
}

TEST(Branch, UnsignedComparisons) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // Max Unsigned
    sim.write_reg(X6, 1);
    sim.set_pc(0x100);

    // BLTU: MaxU < 1 is False
    sim.execute_instr(rv::Instruction(rv::BLTU{X5, X6, 0x20}));
    EXPECT_EQ(sim.pc(), 0x104);

    // BGEU: MaxU >= 1 is True
    sim.set_pc(0x100);
    sim.execute_instr(rv::Instruction(rv::BGEU{X5, X6, 0x20}));
    EXPECT_EQ(sim.pc(), 0x120);
}

// --- Jump Tests ---
TEST(Jump, JAL) {
    auto sim = create_sim();
    sim.set_pc(0x100);

    // Jump 0x20 bytes forward, save return addr in RA (X1)
    sim.execute_instr(rv::Instruction(rv::JAL{X1, 0x20}));
    
    EXPECT_EQ(sim.pc(), 0x120);
    EXPECT_EQ(sim.read_reg(X1), 0x104);
}

TEST(Jump, JALR) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x2000);
    sim.set_pc(0x100);

    // Jump to X5 + 4, save return addr in X1
    sim.execute_instr(rv::Instruction(rv::JALR{X1, X5, 4}));
    
    EXPECT_EQ(sim.pc(), 0x2004);
    EXPECT_EQ(sim.read_reg(X1), 0x104);
}


// Pseudo

TEST(Pseudo, LI) {
    auto sim = create_sim();
    
    // Emulating 'li x5, 42' using ADDI
    sim.execute_instr(rv::Instruction(rv::ADDI{X5, X0, 42}));
    EXPECT_EQ(sim.read_reg(X5), 42);

    // Test negative LI: 'li x6, -1'
    sim.execute_instr(rv::Instruction(rv::ADDI{X6, X0, -1}));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFFFFFF);
}

// --- Other ---
TEST(Integration, SimpleEquation) {
    auto sim = create_sim();
    
    // li t0, 5  (represented as ADDI x5, x0, 5)
    // li t1, 10
    // add t2, t0, t1
    // sub a0, t2, t0
    
    sim.execute_instr(rv::Instruction(rv::ADDI{X5, X0, 5}));
    sim.execute_instr(rv::Instruction(rv::ADDI{X6, X0, 10}));
    sim.execute_instr(rv::Instruction(rv::ADD{X7, X5, X6}));
    sim.execute_instr(rv::Instruction(rv::SUB{X10, X7, X5}));

    EXPECT_EQ(sim.read_reg(X10), 10);
    EXPECT_EQ(sim.pc(), 16); // 4 instructions * 4 bytes
}


