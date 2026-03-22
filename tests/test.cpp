#include <gtest/gtest.h>

#include "simulator.hpp"

static const char isa_str[] = "rv32i";

static const rv::Config config = 
{
    .isa = "rv32i",
    .elf_path = "",
    .init_state_path = "",
    .final_state_path = "",
    .interactive = false,
    .logs_disabled = false,
    .steps = 10
};

static const rv::Parser::SegmentInfo DEFAULT_TEXT_SEGMENT = 
{
    .vaddr = 0x10000,
    .memsz = 128,
    .filesz = 128,
    .data = std::vector<uint8_t>(128),
    .r = true,
    .w = false,
    .x = true
};

static const rv::Parser::SegmentInfo DEFAULT_DATA_SEGMENT = 
{
    .vaddr = 0x20000,
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
    rv::Simulator sim(config);
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
    
    sim.execute_instr(rv::ADD(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 150);
}

TEST(Arithmetic, SUB) {
    auto sim = create_sim();
    sim.write_reg(X5, 100);
    sim.write_reg(X6, 30);
    
    sim.execute_instr(rv::SUB(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 70);
}

TEST(Arithmetic, SLT_Signed) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); // -1
    sim.write_reg(X6, 1);          //  1
    
    sim.execute_instr(rv::SLT(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 1); 
}

TEST(Arithmetic, SLTU_Unsigned) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); 
    sim.write_reg(X6, 1);
    
    sim.execute_instr(rv::SLTU(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 0); 
}

TEST(Arithmetic, Bitwise) {
    auto sim = create_sim();
    sim.write_reg(X5, 0b1010);
    sim.write_reg(X6, 0b1100);
    
    sim.execute_instr(rv::AND(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 0b1000);

    sim.execute_instr(rv::OR(X8, X5, X6));
    EXPECT_EQ(sim.read_reg(X8), 0b1110);

    sim.execute_instr(rv::XOR(X9, X5, X6));
    EXPECT_EQ(sim.read_reg(X9), 0b0110);
}

TEST(Arithmetic, Shifts) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xF0000000);
    sim.write_reg(X6, 4);
    
    sim.execute_instr(rv::SRL(X7, X5, X6));
    EXPECT_EQ(sim.read_reg(X7), 0x0F000000);

    sim.execute_instr(rv::SRA(X8, X5, X6));
    EXPECT_EQ(sim.read_reg(X8), 0xFF000000);
    
    sim.write_reg(X5, 0x1);
    sim.execute_instr(rv::SLL(X9, X5, X6));
    EXPECT_EQ(sim.read_reg(X9), 0x10);
}

// --- I-Type Arithmetic Tests ---

TEST(ArithmeticImm, ADDI) {
    auto sim = create_sim();
    sim.write_reg(X5, 10);
    
    sim.execute_instr(rv::ADDI(X6, X5, -15));
    EXPECT_EQ(sim.read_reg(X6), (uint32_t)-5);
}

TEST(ArithmeticImm, SLTI_SLTIU) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); 
    
    sim.execute_instr(rv::SLTI(X6, X5, 0));
    EXPECT_EQ(sim.read_reg(X6), 1);

    sim.execute_instr(rv::SLTIU(X7, X5, 2047));
    EXPECT_EQ(sim.read_reg(X7), 0);
}

TEST(ArithmeticImm, XORI_ORI_ANDI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x0FF);
    
    sim.execute_instr(rv::ANDI(X6, X5, 0x00F));
    EXPECT_EQ(sim.read_reg(X6), 0x00F);

    sim.execute_instr(rv::ORI(X7, X5, 0xF00));
    EXPECT_EQ(sim.read_reg(X7), 0xFFF);
}

TEST(ArithmeticImm, SLLI_SRLI_SRAI) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x80000000);
    
    sim.execute_instr(rv::SRAI(X6, X5, 1));
    EXPECT_EQ(sim.read_reg(X6), 0xC0000000);
    
    sim.write_reg(X10, 0x1);
    sim.execute_instr(rv::SLLI(X11, X10, 5));
    EXPECT_EQ(sim.read_reg(X11), 32);
}

TEST(Arithmetic, ZeroRegisterProtection) {
    auto sim = create_sim();
    sim.write_reg(X1, 100);
    sim.write_reg(X2, 100);
    
    sim.execute_instr(rv::ADD(X0, X1, X2));
    EXPECT_EQ(sim.read_reg(X0), 0);
}

// --- UpperImmediate ---

TEST(UpperImmediate, LUI) {
    auto sim = create_sim();
    sim.execute_instr(rv::LUI(X10, 0x12345));
    EXPECT_EQ(sim.read_reg(X10), 0x12345000);
}

TEST(UpperImmediate, AUIPC) {
    auto sim = create_sim();
    sim.set_pc(0x1000);
    
    sim.execute_instr(rv::AUIPC(X10, 0x2)); 
    EXPECT_EQ(sim.read_reg(X10), 0x1000 + 0x2);
}

// --- Loads & Stores Tests ---

TEST(Memory, LoadStoreByte) {
    auto sim = create_sim();
    uint32_t base_addr = DEFAULT_DATA_SEGMENT.vaddr;
    
    sim.write_reg(X5, 0xABCDEF88);
    sim.write_reg(X1, base_addr);
    
    sim.execute_instr(rv::SB(X1, X5, 0));
    
    sim.execute_instr(rv::LB(X6, X1, 0));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFFFF88);

    sim.execute_instr(rv::LBU(X7, X1, 0));
    EXPECT_EQ(sim.read_reg(X7), 0x00000088);
}

TEST(Memory, LoadStoreWord) {
    auto sim = create_sim();
    uint32_t base_addr = DEFAULT_DATA_SEGMENT.vaddr + 10;
    
    sim.write_reg(X10, 0xDEADBEEF);
    sim.write_reg(X1, base_addr);

    sim.execute_instr(rv::SW(X1, X10, 0));
    sim.execute_instr(rv::LW(X11, X1, 0));
    
    EXPECT_EQ(sim.read_reg(X11), 0xDEADBEEF);
}

TEST(Memory, HalfWordLoadStore) {
    auto sim = create_sim();
    uint32_t base_addr = DEFAULT_DATA_SEGMENT.vaddr + 41;
    
    sim.write_reg(X5, 0x8001); 
    sim.write_reg(X1, base_addr);

    sim.execute_instr(rv::SH(X1, X5, 0));

    sim.execute_instr(rv::LH(X6, X1, 0));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFF8001);

    sim.execute_instr(rv::LHU(X7, X1, 0));
    EXPECT_EQ(sim.read_reg(X7), 0x00008001);
}

// --- Branch Tests ---

TEST(Branch, ComparisonLogic) {
    auto sim = create_sim();
    sim.set_pc(0x100);
    sim.write_reg(X5, 10);
    sim.write_reg(X6, 20);

    sim.execute_instr(rv::BEQ(X5, X6, 100));
    EXPECT_EQ(sim.pc(), 0x104);

    sim.set_pc(0x100);
    sim.execute_instr(rv::BNE(X5, X6, 100));
    EXPECT_EQ(sim.pc(), 0x164);

    sim.set_pc(0x100);
    sim.execute_instr(rv::BLT(X5, X6, 100));
    EXPECT_EQ(sim.pc(), 0x164);
}

TEST(Branch, BGE_Signed) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); 
    sim.write_reg(X6, 1);          
    sim.set_pc(0x100);

    sim.execute_instr(rv::BGE(X5, X6, 0x20));
    EXPECT_EQ(sim.pc(), 0x104);

    sim.set_pc(0x100);
    sim.execute_instr(rv::BGE(X6, X5, 0x20));
    EXPECT_EQ(sim.pc(), 0x120);
}

TEST(Branch, UnsignedComparisons) {
    auto sim = create_sim();
    sim.write_reg(X5, 0xFFFFFFFF); 
    sim.write_reg(X6, 1);
    sim.set_pc(0x100);

    sim.execute_instr(rv::BLTU(X5, X6, 0x20));
    EXPECT_EQ(sim.pc(), 0x104);

    sim.set_pc(0x100);
    sim.execute_instr(rv::BGEU(X5, X6, 0x20));
    EXPECT_EQ(sim.pc(), 0x120);
}

// --- Jump Tests ---

TEST(Jump, JAL) {
    auto sim = create_sim();
    sim.set_pc(0x100);

    sim.execute_instr(rv::JAL(X1, 0x20));
    EXPECT_EQ(sim.pc(), 0x120);
    EXPECT_EQ(sim.read_reg(X1), 0x104);
}

TEST(Jump, JALR) {
    auto sim = create_sim();
    sim.write_reg(X5, 0x2000);
    sim.set_pc(0x100);

    sim.execute_instr(rv::JALR(X1, X5, 4));
    EXPECT_EQ(sim.pc(), 0x2004);
    EXPECT_EQ(sim.read_reg(X1), 0x104);
}

// --- Pseudo ---

TEST(Pseudo, LI) {
    auto sim = create_sim();
    sim.execute_instr(rv::LI(X5, 42));
    EXPECT_EQ(sim.read_reg(X5), 42);

    sim.execute_instr(rv::LI(X6, -1));
    EXPECT_EQ(sim.read_reg(X6), 0xFFFFFFFF);
}

// --- Integration ---

TEST(Integration, SimpleEquation) {
    auto sim = create_sim();
    
    sim.execute_instr(rv::LI(X5, 5));
    sim.execute_instr(rv::LI(X6, 10));
    sim.execute_instr(rv::ADD(X7, X5, X6));
    sim.execute_instr(rv::SUB(X10, X7, X5));

    EXPECT_EQ(sim.read_reg(X10), 10);
    EXPECT_EQ(sim.pc(), 16); 
}