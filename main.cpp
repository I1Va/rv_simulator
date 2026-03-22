#include <iostream>
#include <string>
#include "simulator.hpp"
#include "CLI11.hpp"

std::string get_config_str(rv::Config &config) {
    std::stringstream ss;
    ss << "========================================\n"
        << " Simulation Configuration:\n"
        << "----------------------------------------\n"
        << "  ISA Model    : " << config.isa << "\n"
        << "  ELF Path     : " << (config.elf_path.empty() ? "None" : config.elf_path) << "\n"
        << "  Init State   : " << (config.init_state_path.empty() ? "Disabled" : config.init_state_path) << "\n"
        << "  Step Limit   : " << config.steps << "\n"
        << "  Interactive  : " << config.interactive << "\n"
        << "========================================";
    return ss.str();
}

int main(int argc, char* argv[]) {
    rv::Config config;
    CLI::App app{"RISC-V RV32I Simulator"};

    app.add_option("elf", config.elf_path, "Path to the target ELF binary")
       ->required()
       ->check(CLI::ExistingFile);

    app.add_option("--isa", config.isa, "Set ISA model (default: rv32i)");

    app.add_option("--steps,-s", config.steps, "Number of instructions to execute")
       ->capture_default_str();

    app.add_option("--init_state,-i", config.init_state_path, "Path to initial PC and registers file")
       ->check(CLI::ExistingFile);

    app.add_option("--final_state,-f", config.final_state_path, "Path to final PC and registers file");

    app.add_flag("--interactive", config.interactive, "Enable interactive mode");
    app.add_flag("--disable-logs", config.logs_disabled, "Enable interactive mode");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    try {
        rv::Simulator sim(config);

        if (sim.load_elf(config.elf_path) != 0) {
            return 1;
        }

        if (!config.init_state_path.empty()) {
            if (sim.load_initial_state(config.init_state_path) != 0) {
                std::cerr << "Error: Failed to load initial state from " << config.init_state_path << "\n";
                return 1;
            }

            if (!config.logs_disabled)
            std::cout << "Initial state loaded from: " << config.init_state_path << "\n";
        }

        if (!config.logs_disabled) {
            std::cout << "Starting simulation: \n";
            std::cout << get_config_str(config) << "\n";
        }   
        
        if (config.interactive) {
            sim.interactive_mode();
        } else {
            sim.run(config.steps);
        }

        if (!config.final_state_path.empty()) {
            if (sim.dump_cpu_state(config.final_state_path) != 0) {
                std::cerr << "Error: Failed to write final state to " << config.final_state_path << "\n";
                return 1;
            }
        }
    
    } catch (const std::exception& e) {
        std::cerr << "Simulator creation failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}