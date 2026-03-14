#include <iostream>
#include <string>
#include <vector>
#include "simulator.hpp"

struct Config {
    std::string isa = "rv32i"; 
    std::string elf_path = "";
};

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <elf_file>\n"
              << "Options:\n"
              << "  --isa=<model>    Set ISA model (default: rv32i)\n"
              << "  --help           Show this message\n";
}

int main(int argc, char* argv[]) {
    Config config;
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty()) {
        print_usage(argv[0]);
        return 1;
    }

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg.find("--isa=") == 0) {
            config.isa = arg.substr(6);
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
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
                  << " (ISA: " << config.isa << ")\n";

        rv::Simulator sim(config.isa);
        if (sim.load_elf(config.elf_path)) {
            return 1;
        }
        
        sim.sim_step();
        
    } catch (const std::exception& e) {
        std::cerr << "Simulation aborted: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
