#pragma once

class Instruction {
    // instrcution interface

    // uint64_t addr; uint8_t size; uint32_t raw;
    // virtual ~Instruction() = default;
    // virtual void execute(ICpu& cpu, IMemory& mem) const = 0;    
};


class InstrAdd : Instruction {}; // etc
