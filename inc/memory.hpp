#pragma once

#include "utility.hpp"

namespace rv 
{

class MemoryException : public std::runtime_error {
public:
    MemoryException(std::string_view msg, uint64_t addr) 
        : std::runtime_error(std::format("{} at address 0x{:016x}", msg, addr)) 
    {}
};

class AccessFault : public MemoryException {
public:
    AccessFault(uint64_t a) 
        : MemoryException(std::format("Access Fault: Permission denied at 0x{:08x}", a), a) {}
};

// Case: No segment exists at this address
class PageFault : public MemoryException {
public:
    PageFault(uint32_t a) 
        : MemoryException(std::format("Page Fault: No segment mapped at 0x{:08x}", a), a) {}
};

class InstructionAddressMisaligned : public MemoryException {
public:
    InstructionAddressMisaligned(uint64_t a) 
        : MemoryException(std::format("Instruction Address Misaligned: 0x{:08x}", a), a) {}
};

class BoundaryFault : public MemoryException {
public:
    BoundaryFault(uint64_t a) 
        : MemoryException(std::format("Boundary Fault: 32-bit access at 0x{:08x} exceeds segment limit", a), a) {}
};

class IMEM {
    // it is high abstraction interface, does not take into account memory model details
    // contain raw bytes of data
    // alow to read, write
public:
    virtual void add_segment
    (
        uint64_t addr, uint64_t size, 
        bool r, bool w, bool x, 
        const std::vector<uint8_t>& init_data
    ) = 0;

    virtual ~IMEM() = default;
    // virtual uint8_t read8(uint64_t a) = 0;
    virtual uint32_t read32(uint64_t a) const = 0 ;
    virtual uint32_t read_instr32(uint64_t a) const = 0;
    virtual void write32(uint64_t a, uint32_t v) = 0;
};

class MEM32 : public IMEM {
    struct Segment {
        uint32_t vaddr;
        uint32_t memsz;
        std::vector<uint8_t> data;
        
        bool r{false};
        bool w{false};
        bool x{false};

        bool contains(uint32_t addr) const {
            return addr >= vaddr && addr < (vaddr + memsz);
        }
    };

    std::vector<Segment> segments;

    Segment& get_seg(uint32_t addr, bool write_access, bool execute_access) {
        for (auto& seg : segments) {
            if (seg.contains(addr)) {
                if ((write_access && !seg.w) || (execute_access && !seg.x)) {
                    throw AccessFault(addr);
                }
                return seg;
            }
        }
        throw PageFault(addr);
    }

    const Segment& get_seg(uint32_t addr, bool write_access, bool execute_access) const {
         for (auto& seg : segments) {
            if (seg.contains(addr)) {
                if ((write_access && !seg.w) || (execute_access && !seg.x)) {
                    throw AccessFault(addr);
                }
                return seg;
            }
        }
        throw PageFault(addr);
    }


public:
    MEM32() = default;

    void add_segment
    (
        uint64_t addr, uint64_t size, 
        bool r, bool w, bool x, 
        const std::vector<uint8_t>& init_data
    ) override {
        segments.emplace_back(
            Segment
            {
                static_cast<uint32_t>(addr),
                static_cast<uint32_t>(size),
                std::move(init_data),
                r, w, x
            }
        );
        if (segments.back().data.size() < size) {
            segments.back().data.resize(size, 0);
        }
    }

    void write32(uint64_t addr, uint32_t v) override {
        Segment &segment = get_seg(addr, /*write_access=*/true, /*execute=*/false);
        
        uint64_t offset = addr - segment.vaddr;

        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(addr);
        }

        // Little-Endian Write
        segment.data[offset + 0] = static_cast<uint8_t>(v & 0xFF);         // Byte 0 (LSB)
        segment.data[offset + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);  // Byte 1
        segment.data[offset + 2] = static_cast<uint8_t>((v >> 16) & 0xFF); // Byte 2
        segment.data[offset + 3] = static_cast<uint8_t>((v >> 24) & 0xFF); // Byte 3 (MSB)
    }

    uint32_t read32(uint64_t a) const override {
        const Segment &segment = get_seg(a, /*write=*/false, /*execute=*/false);
        
        uint64_t offset = a - segment.vaddr;
    
        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(static_cast<uint32_t>(a));
        }

        // Reconstruct 32-bit word (Little-Endian)
        return static_cast<uint32_t>(segment.data[offset + 0])      |
            (static_cast<uint32_t>(segment.data[offset + 1]) << 8)  |
            (static_cast<uint32_t>(segment.data[offset + 2]) << 16) |
            (static_cast<uint32_t>(segment.data[offset + 3]) << 24);
    }

    uint32_t read_instr32(uint64_t a) const override {
        const Segment& segment = get_seg(a, /*write=*/false, /*execute=*/true);
        
        uint64_t offset = a - segment.vaddr;
        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(static_cast<uint32_t>(a));
        }

        // Reconstruct 32-bit word (Little-Endian)
        return static_cast<uint32_t>(segment.data[offset + 0])      |
            (static_cast<uint32_t>(segment.data[offset + 1]) << 8)  |
            (static_cast<uint32_t>(segment.data[offset + 2]) << 16) |
            (static_cast<uint32_t>(segment.data[offset + 3]) << 24);
        }
    };

} // namespace rv