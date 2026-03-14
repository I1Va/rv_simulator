#pragma once
#include <memory>

#include "parser.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "instruction.hpp"

namespace rv 
{

class Simulator {
    std::unique_ptr<ICPU> cpu_;
    std::unique_ptr<IMEM> mem_;
    // std::unique_ptr<IDecoder> decoder;

public:
    Simulator(std::string_view isa_string) {

        if (isa_string == "rv32i") {
            cpu_ = std::make_unique<CPU_RV32I>();
            mem_ = std::make_unique<MEM32>();
            // decoder = std::make_unique<Decoder_RV32I>();
        } else {
            throw std::runtime_error("Unknown ISA configuration");
        }
    }

    int load_elf(const std::string_view elf_path) {
        try {
            Parser loader(elf_path);
            loader.dump();

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

    void sim_run() {
        // Instrction
    }

    // launch parser
    // contain CPU, MEM interfaces
    // iterate though instrcutions, execute them and update CPU, MEM state
    // can dump inner state  
};

} // namespace rv 
