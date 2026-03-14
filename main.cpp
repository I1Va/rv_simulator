#include <iostream>
#include <string>
#include <vector>
#include "simulator.hpp"

struct Config {
    std::string isa = "rv32i"; 
    std::string elf_path = "";
    int steps = 10;
};

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <elf_file>\n"
              << "Options:\n"
              << "  --isa=<model>    Set ISA model (default: rv32i)\n"
              << "  --s <steps>      Number of instructions to execute\n"
              << "  --help           Show this message\n";
}

int main(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty()) {
        print_usage(argv[0]);
        return 1;
    }

   for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--s" || arg == "-s") {
            if (i + 1 < argc) {
                try {
                    config.steps = std::stoull(argv[++i]);
                } catch (...) {
                    std::cerr << "Error: '" << argv[i] << "' is not a valid number of steps.\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: " << arg << " requires a number.\n";
                return 1;
            }
        } else if (arg.find("--isa=") == 0) {
            config.isa = arg.substr(6);
        } else {
            config.elf_path = arg;
        }
    }

    if (config.elf_path.empty()) {
        std::cerr << "Error: No target binary specified.\n";
        return 1;
    }

    try {
        std::cout << "Starting simulation: " << config.elf_path  
                  << " (ISA : " << config.isa   << ","
                  << " steps : " << config.steps << ")\n";

        rv::Simulator sim(config.isa);
        if (sim.load_elf(config.elf_path)) {
            return 1;
        }
        
        sim.run(config.steps);
       
    } catch (const std::exception& e) {
        std::cerr << "Simulation aborted: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
