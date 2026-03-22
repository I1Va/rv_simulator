#pragma once

#include <cstdint>
#include <cassert>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>

#include "memory.hpp"
#include "decoder.hpp"
#include "instruction.hpp"
#include "RV32I.hpp"

namespace rv
{

class UnknownSyscall : public std::runtime_error {
public:
    explicit UnknownSyscall(uint32_t syscall_num) 
        : std::runtime_error(std::format("Unknown or unsupported syscall: a7 = {}", syscall_num)),
          syscall_num_(syscall_num) 
    {}

    uint32_t get_syscall_num() const noexcept { return syscall_num_; }

private:
    uint32_t syscall_num_;
};
    

class ICPU {
public:    
    virtual uint64_t read_reg(uint8_t idx) const = 0;
    virtual void write_reg(uint8_t idx, uint64_t val) = 0;
    virtual void set_pc(uint64_t pc) = 0;
    virtual uint64_t pc() const = 0;
    virtual void dump() const = 0;
    virtual void execute(const Instruction &i, IMEM &m) = 0;
    virtual Instruction fetch_and_decode(uint64_t addr, IMEM &mem) const = 0;
    virtual bool is_running() const = 0;

    // virtual void raise_exception(Exception e) = 0;
};

class CPU_RV32I : public ICPU {
    Config config_;

    uint32_t pc_;
    std::vector<uint32_t> regs_;
    std::unique_ptr<IDecoder> decoder_;
    bool is_running_ = true;

    uint8_t buffer_[BUFSIZ] = {};

public:
    CPU_RV32I(const Config &config): config_(config), pc_(0), regs_(32) {
        decoder_ = std::make_unique<Decoder_RV32I>(config_);
    }

    void set_pc(uint64_t pc) override {
        pc_ = static_cast<uint32_t> (pc);
    }

    uint64_t pc() const override {
        return pc_;
    }

    Instruction fetch_and_decode(uint64_t addr, IMEM &mem) const override {
        return decoder_->fetch_and_decode(addr, mem);
    }
    
    uint64_t read_reg(uint8_t idx) const override {
        assert(idx < 32 && "Register index out of range!");
        return regs_[idx];
    }

    void write_reg(uint8_t idx, uint64_t val) override {
        assert(idx < 32 && "Register index out of range!");
        if (idx == 0) return;
        regs_[idx] = static_cast<uint32_t>(val);
    }

    bool is_running() const override { return is_running_; }

    void dump() const {
        std::cout << " pc       " 
                << std::hex << std::setw(8) << std::setfill('0') << pc_ 
                << std::dec << "\n";

        for (int i = 0; i < 32; ++i) {
            std::string abi = "x" + std::to_string(i) + "/" + reg_names[i];
            
            std::cout << " ";

            std::cout << std::left << std::setw(8) << std::setfill(' ') << abi << " ";
            
            std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << regs_[i];

            if ((i + 1) % 4 == 0) {
                std::cout << "\n";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::dec << std::setfill(' ');
    }

    void execute(const Instruction &i, IMEM &m) override {
        switch (i.type) {
            // --- Arithmetic R-Type ---
            case InstructionType::ADD:
                write_reg(i.rd, read_reg(i.rs1) + read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SUB:
                write_reg(i.rd, read_reg(i.rs1) - read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SLL:
                write_reg(i.rd, read_reg(i.rs1) << (read_reg(i.rs2) & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SLT:
                write_reg(i.rd, (int32_t)read_reg(i.rs1) < (int32_t)read_reg(i.rs2) ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTU:
                write_reg(i.rd, read_reg(i.rs1) < read_reg(i.rs2) ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::XOR:
                write_reg(i.rd, read_reg(i.rs1) ^ read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRL:
                write_reg(i.rd, read_reg(i.rs1) >> (read_reg(i.rs2) & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRA:
                write_reg(i.rd, (uint32_t)((int32_t)read_reg(i.rs1) >> (read_reg(i.rs2) & 0x1F)));
                set_pc(pc() + 4);
                break;
            case InstructionType::OR:
                write_reg(i.rd, read_reg(i.rs1) | read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::AND:
                write_reg(i.rd, read_reg(i.rs1) & read_reg(i.rs2));
                set_pc(pc() + 4);
                break;

            // --- Arithmetic I-Type ---
            case InstructionType::ADDI:
                write_reg(i.rd, read_reg(i.rs1) + i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTI:
                write_reg(i.rd, (int32_t)read_reg(i.rs1) < i.imm ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLTIU:
                write_reg(i.rd, read_reg(i.rs1) < (uint32_t)i.imm ? 1 : 0);
                set_pc(pc() + 4);
                break;
            case InstructionType::XORI:
                write_reg(i.rd, read_reg(i.rs1) ^ (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::ORI:
                write_reg(i.rd, read_reg(i.rs1) | (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::ANDI:
                write_reg(i.rd, read_reg(i.rs1) & (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::SLLI:
                write_reg(i.rd, read_reg(i.rs1) << (i.shamt & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRLI:
                write_reg(i.rd, read_reg(i.rs1) >> (i.shamt & 0x1F));
                set_pc(pc() + 4);
                break;
            case InstructionType::SRAI:
                write_reg(i.rd, (uint32_t)((int32_t)read_reg(i.rs1) >> (i.shamt & 0x1F)));
                set_pc(pc() + 4);
                break;

            // --- Memory Operations ---
            case InstructionType::LB:
                write_reg(i.rd, (int32_t)(int8_t)m.read8(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LH:
                write_reg(i.rd, (int32_t)(int16_t)m.read16(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LW:
                write_reg(i.rd, m.read32(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LBU:
                write_reg(i.rd, (uint32_t)m.read8(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::LHU:
                write_reg(i.rd, (uint32_t)m.read16(read_reg(i.rs1) + i.imm));
                set_pc(pc() + 4);
                break;
            case InstructionType::SB:
                m.write8(read_reg(i.rs1) + i.imm, (uint8_t)read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SH:
                m.write16(read_reg(i.rs1) + i.imm, (uint16_t)read_reg(i.rs2));
                set_pc(pc() + 4);
                break;
            case InstructionType::SW:
                m.write32(read_reg(i.rs1) + i.imm, read_reg(i.rs2));
                set_pc(pc() + 4);
                break;

            // --- Control Flow ---
            case InstructionType::BEQ:
                set_pc(pc() + (read_reg(i.rs1) == read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BNE:
                set_pc(pc() + (read_reg(i.rs1) != read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BLT:
                set_pc(pc() + ((int32_t)read_reg(i.rs1) < (int32_t)read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BGE:
                set_pc(pc() + ((int32_t)read_reg(i.rs1) >= (int32_t)read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BLTU:
                set_pc(pc() + (read_reg(i.rs1) < read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::BGEU:
                set_pc(pc() + (read_reg(i.rs1) >= read_reg(i.rs2) ? i.imm : 4));
                break;
            case InstructionType::JAL:
                write_reg(i.rd, pc() + 4);
                set_pc(pc() + i.imm);
                break;
            case InstructionType::JALR: {
                uint32_t target = (read_reg(i.rs1) + i.imm) & ~1U;
                write_reg(i.rd, pc() + 4);
                set_pc(target);
                break;
            }

            // --- Upper Immediates & Pseudo ---
            case InstructionType::LUI:
                write_reg(i.rd, (uint32_t)i.imm << 12);
                set_pc(pc() + 4);
                break;
            case InstructionType::AUIPC:
                write_reg(i.rd, (uint32_t)pc() + (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;
            case InstructionType::LI:
                write_reg(i.rd, (uint32_t)i.imm);
                set_pc(pc() + 4);
                break;

            case InstructionType::ECALL:
                {
                uint32_t syscall_num = read_reg(Reg::a7); 
                switch (syscall_num) {
                    case 63: // 'read' syscall
                        handle_read(m);
                        break;
                    case 64: // 'read' syscall
                        handle_write(m);
                        break;
                    case 93: // 'exit' syscall
                        handle_exit();
                        break;
                    default:
                        throw UnknownSyscall(syscall_num);
                }
                set_pc(pc() + 4);
                break;
                }
            
            case InstructionType::EBREAK: // TODO BREAK
                set_pc(pc() + 4);
                break;
            
            case InstructionType::FENCE:
            case InstructionType::FENCE_I:
                // nop
                set_pc(pc() + 4);
                break;


            default:
                throw std::runtime_error(std::format(
                    "CPU_RV32I::execute unknown instruction type {}", 
                    static_cast<int>(i.type)
                ));
        }
    }

private:
    void handle_read(IMEM &mem) {
        uint64_t fd        = read_reg(Reg::a0);
        uint64_t vaddr     = read_reg(Reg::a1);
        uint64_t total_max = read_reg(Reg::a2);
        uint64_t total_read = 0;

        while (total_read < total_max) {
            uint64_t remaining = total_max - total_read;
            uint64_t chunk_size = std::min(remaining, (uint64_t)sizeof(buffer_));

            long ret;
            asm volatile (
                "xor %%rax, %%rax;" 
                "syscall;"
                : "=a"(ret)
                : "D"(fd), "S"(buffer_), "d"(chunk_size)
                : "rcx", "r11", "memory"
            );

            if (ret < 0) {
                if (total_read == 0) write_reg(Reg::a0, static_cast<uint64_t>(ret));
                else write_reg(Reg::a0, total_read);
                return;
            }
            
            if (ret == 0) break;

            for (long i = 0; i < ret; ++i) {
                mem.write8(vaddr + total_read + i, buffer_[i]);
            }

            total_read += ret;

            if (ret < (long)chunk_size) break;
        }

        write_reg(Reg::a0, total_read);
    }

    void handle_write(IMEM &mem) {
        uint64_t fd      = read_reg(Reg::a0);
        uint64_t vaddr   = read_reg(Reg::a1);
        uint64_t total_n = read_reg(Reg::a2);
        uint64_t total_written = 0;
        while (total_written < total_n) {
            uint64_t chunk_size = std::min(total_n - total_written, sizeof(buffer_) / sizeof(uint8_t));
            
            for (uint64_t i = 0; i < chunk_size; ++i) {
                buffer_[i] = mem.read8(vaddr + total_written + i);
            }

            long ret;
            asm volatile (
                "mov $1, %%rax;"  
                "syscall;"
                : "=a"(ret)
                : "D"(fd), "S"(buffer_), "d"(chunk_size)
                : "rcx", "r11", "memory"
            );

            if (ret < 0) {
                write_reg(Reg::a0, static_cast<uint64_t>(ret)); // Return error code
                return;
            }
            
            total_written += ret;
            if (ret < (long)chunk_size) break; // Partial write
        }
        write_reg(Reg::a0, total_written);
    }

    void handle_exit() {
        if (!config_.logs_disabled)
        std::cout << "[SIMULATOR] Program exited with status code: " << (int64_t) read_reg(Reg::a0) << "\n";
        is_running_ = false;
    }           
};


} // namespace rv
