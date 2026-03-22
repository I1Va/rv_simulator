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
public:
    Simulator(std::string_view isa_string) {
        if (isa_string == "rv32i") {
            cpu_ = std::make_unique<CPU_RV32I>();
            mem_ = std::make_unique<MEM32>();
        } else {
            throw std::runtime_error("Unknown ISA configuration");
        }
    }

    int load_initial_state(const std::string_view init_state_path) {
        std::ifstream file(init_state_path.data());
        if (!file.is_open()) {
            std::cerr << "Error: Could not open state file " << init_state_path << "\n";
            return -1;
        }

        std::string val;
        std::vector<uint64_t> values;
        bool write_pc = true;

        size_t reg_idx = 0;
        while (file >> val) {
            try {
                if (reg_idx == 0 && val == "@") {
                    write_pc = false;   
                } else {
                    values.push_back(static_cast<uint64_t>(std::stoul(val, nullptr, 0)));
                }  
                reg_idx++;
            } catch (...) {
                std::cerr << "Error: Invalid numeric value in state file: " << val << "\n";
                return -1;
            }
        }

        if (reg_idx < 33) {
            std::cerr << "Error: State file too short. Expected 33 values, got " << values.size() << "\n";
            return -1;
        }


        if (write_pc) cpu_->set_pc(values[0]);

        for (size_t i = 0; i < 32; ++i) {
            cpu_->write_reg(i, values[i]);
        }

        return 0;
    }

    int dump_cpu_state(const std::string_view dump_path) {
        std::ofstream file(dump_path.data());
        if (!file.is_open()) {
            std::cerr << "Error: Could not open cpu state dump file " << dump_path << "\n";
            return -1;
        }

        file << "0x" << std::hex << std::setw(8) << std::setfill('0') << cpu_->pc() << "\n";
        for (size_t i = 0; i < 32; i++) {
            file << "0x" << std::hex << std::setw(8) << std::setfill('0') << cpu_->read_reg(i) << "\n";
        }

        return 0;
    }

    int load_elf(const std::string_view elf_path) {
        try {
            Parser parser;
            parser.load_elf(elf_path);

            cpu_->set_pc(parser.get_entry_point());

            for (const auto& seg : parser.get_segments()) {
                mem_->add_segment(seg);
            }

            return 0; 
        } 
        catch (const std::exception& e) {
            std::cerr << "Loader Error: " << e.what() << std::endl;
            return 1;
        }
    }

    void execute_instr(const Instruction &instruction) {
        cpu_->execute(instruction, *mem_.get());
    }

    void set_pc(const uint64_t pc) { cpu_->set_pc(pc); }
    void add_segment
    (
        const Parser::SegmentInfo &segment_info
    ) { mem_->add_segment(segment_info); }

    void run(const size_t steps) {
        for (size_t i = 0; i < steps; i++) {
            step();
        }
    }

    void step() {
        try {
            Instruction instruction = cpu_->fetch_and_decode(cpu_->pc(), *mem_.get()); 
            cpu_dump();
        
            cpu_->execute(instruction, *mem_.get());
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

    uint64_t pc() {
       return cpu_->pc();
    }
    
    std::vector<uint64_t> regs() {
        std::vector<uint64_t> regs(32);
        for (size_t i = 0; i < 32; i++) {
            regs[i] = cpu_->read_reg(i);
        }
        return regs;
    }

    void write_reg(const uint64_t idx, const uint64_t value) {
        cpu_->write_reg(idx, value);
    }
    uint64_t read_reg(const uint64_t idx) const {
        return cpu_->read_reg(idx);
    }

    void cpu_dump() {
        cpu_->dump();
    }
    
    void interactive_mode() {
        std::string line;
        std::cout << "Interactive Mode: type 'h' for help\n";

        while (true) {
            std::cout << "sim> ";
            if (!std::getline(std::cin, line) || line == "q" || line == "exit") break;
            if (line.empty()) continue;

            if (line == "h" || line == "help") {
                std::cout << "Commands:\n"
                        << "  s [n]       Step n instructions (default 1)\n"
                        << "  r           Dump registers\n"
                        << "  m <addr>    Dump memory at address\n"
                        << "  p <addr>    Set PC to address\n"
                        << "  q           Quit\n";
            } 
            else if (line[0] == 's') {
                int steps = 1;
                if (line.size() > 2) steps = std::stoi(line.substr(2));
                run(steps);
            } 
            else if (line == "r") {
                cpu_dump();
            } 
            else if (line[0] == 'm') {
                try {
                    uint32_t addr = std::stoul(line.substr(2), nullptr, 0);
                    
                    uint32_t word0 = mem_->read32(addr);
                    uint32_t word1 = mem_->read32(addr + 4);

                    std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr << ":  "
                            << "0x" << std::setw(8) << std::setfill('0') << word1 
                            << std::setw(8) << std::setfill('0') << word0 
                            << std::dec << "\n";
                            
                } catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << "\n";
                }
            } 
            else if (line[0] == 'p') {
                try {
                    uint32_t addr = std::stoul(line.substr(2), nullptr, 0);
                    cpu_->set_pc(addr);
                    std::cout << "PC set to: 0x" << std::hex << addr << std::dec << "\n";
                } catch (...) { 
                    std::cout << "Invalid address format\n"; 
                }
            }
        }
    }
};

} // namespace rv 
