#include <iostream>
#include <string>
#include <vector>
#include "simulator.hpp"

struct Config {
    std::string isa = "rv32i"; 
    std::string elf_path = "";
    std::string init_state_path = "";
    int steps = 10;
};

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <elf_file>\n"
              << "Options:\n"
              << "  --isa=<model>    Set ISA model (default: rv32i)\n"
              << "  --s <steps>      Number of instructions to execute\n"
              << "  --init_state <path>  Load initial PC and registers from file\n"
              << "  --help           Show this message\n";
}

int main(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

   for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } 
        else if (arg == "--s" || arg == "-s") {
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
        } 
        else if (arg == "--init_state" || arg == "-is") {
            if (i + 1 < argc) {
                config.init_state_path = argv[++i];
            } else {
                std::cerr << "Error: --init_state requires a file path.\n";
                return 1;
            }
        }
        else if (arg.find("--isa=") == 0) {
            config.isa = arg.substr(6);
        } 
        else {
            config.elf_path = arg;
        }
    }

    if (config.elf_path.empty()) {
        std::cerr << "Error: No target binary specified.\n";
        return 1;
    }

    try {
        rv::Simulator sim(config.isa);
        if (sim.load_elf(config.elf_path) != 0) {
            return 1;
        }
        if (!config.init_state_path.empty()) {
            if (sim.load_initial_state(config.init_state_path) != 0) {
                std::cerr << "Error: Failed to load initial state from " << config.init_state_path << "\n";
                return 1;
            }
            std::cout << "Initial state loaded from: " << config.init_state_path << "\n";
        }

        std::cout << "Starting simulation: " << config.elf_path  
                  << " (ISA : " << config.isa   << ","
                  << " steps : " << config.steps << ")\n";
        sim.run(config.steps);
    } catch (const std::exception& e) {
        std::cerr << "simulator creation failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
