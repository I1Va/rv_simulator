#pragma once
#include <memory>

#include "parser.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include "decoder.hpp"

namespace rv 
{

class Simulator {
    std::unique_ptr<ICPU> cpu_;
    std::unique_ptr<IMEM> mem_;
    std::unique_ptr<IDecoder> decoder_;

public:
    Simulator(std::string_view isa_string) {
        if (isa_string == "rv32i") {
            cpu_ = std::make_unique<CPU_RV32I>();
            mem_ = std::make_unique<MEM32>();
            decoder_ = std::make_unique<Decoder_RV32I>();
        } else {
            throw std::runtime_error("Unknown ISA configuration");
        }
    }

    int load_elf(const std::string_view elf_path) {
        try {
            Parser loader(elf_path);

            cpu_->set_pc(loader.get_entry_point());

            for (const auto& seg : loader.get_segments()) {
                mem_->add_segment(seg.vaddr, seg.memsz, seg.r, seg.w, seg.x, seg.data);
            }

            return 0; 
        } 
        catch (const std::exception& e) {
            std::cerr << "Loader Error: " << e.what() << std::endl;
            return 1;
        }
    }

    void run(const size_t steps) {
        for (size_t i = 0; i < steps; i++) {
            step();
        }
    }

    void step() {
        try {
            Instruction instruction = decoder_->fetch_and_decode(cpu_->pc(), *mem_.get());
            cpu_dump();
        
            execute(instruction, *cpu_.get(), *mem_.get());
        } 
        catch (const rv::IllegalInstruction32PC& e) {
            std::cerr << "[CPU ERROR] " << e.what() << std::endl;
            return;
        } 
        catch (const rv::IllegalInstruction32& e) {
            std::cerr << "[DECODE ERROR] " << e.what() << " at PC: 0x" 
                    << std::hex << cpu_->pc() << std::endl;
            return;
        } 
        catch (const rv::MemoryException& e) {
            std::cerr << "[MEM ERROR] " << e.what() << std::endl;
            return;
        } 

        catch (...) {
            throw; 
        }
    }

    void cpu_dump() {
        cpu_->dump();
    }
};

} // namespace rv 
