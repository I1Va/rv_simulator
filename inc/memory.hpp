#pragma once

#include <unordered_map>
#include <map>
#include <format>
#include <bit>
#include "RV32I.hpp"

#include "utility.hpp"
#include "parser.hpp"

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
    AccessFault(uint64_t addr) 
        : MemoryException(std::format("Access Fault: Permission denied at 0x{:08x}", addr), addr) {}
};

class PageFault : public MemoryException {
public:
    PageFault(uint32_t addr) 
        : MemoryException(std::format("Page Fault: No segment mapped at 0x{:08x}", addr), addr) {}
};

class InstructionAddressMisaligned : public MemoryException {
public:
    InstructionAddressMisaligned(uint64_t addr) 
        : MemoryException(std::format("Instruction Address Misaligned: 0x{:08x}", addr), addr) {}
};

class SegmentAddressMisaligned : public MemoryException {
public:
    SegmentAddressMisaligned(uint64_t addr) 
        : MemoryException("Segment Address Misaligned", addr) {}
};

class SegmentSizeMisaligned : public MemoryException {
public:
    SegmentSizeMisaligned(uint64_t addr, uint64_t sz) 
        : MemoryException(std::format("Segment Size 0x{:08x} Misaligned", sz), addr) {}
};

class SegmentOverlap : public MemoryException {
public:
    SegmentOverlap(uint64_t addr1, uint64_t addr2) 
        : MemoryException(std::format("Segment with vadr 0x{:016x} overlaps", addr1), addr2) {}
};


class BoundaryFault : public MemoryException {
public:
    BoundaryFault(uint64_t addr) 
        : MemoryException(std::format("Boundary Fault: 32-bit access at 0x{:08x} exceeds segment limit", addr), addr) {}
};

class IMEM {
public:
    virtual void add_segment(const Parser::SegmentInfo &segment_info) = 0;

    virtual ~IMEM() = default;

    virtual uint8_t  read8(uint64_t addr) = 0;
    virtual void     write8(uint64_t addr, uint8_t v) = 0;
    virtual uint16_t read16(uint64_t addr) = 0;
    virtual void     write16(uint64_t addr, uint16_t v) = 0;
    virtual uint32_t read32(uint64_t addr) = 0 ;
    virtual void     write32(uint64_t addr, uint32_t v) = 0;
    virtual uint32_t read_instr32(uint64_t addr) = 0;
};

class MEM32 : public IMEM {
    struct Segment {
        uint32_t tag;
        std::vector<uint8_t> data;
        
        bool r{false};
        bool w{false};
        bool x{false};
    };

    std::unordered_map<uint32_t, Segment> segments_;

    const uint32_t alignment_ = PAGE_SIZE;
private:
    Segment& get_seg(uint32_t addr, bool r, bool w, bool x) {
        uint32_t tag = addr / alignment_;

        create_page(tag, r, w, x);
        
        auto segment_it = segments_.find(tag);
        if (r && !segment_it->second.r) throw AccessFault(addr);
        if (w && !segment_it->second.w) throw AccessFault(addr);
        if (x && !segment_it->second.x) throw AccessFault(addr);

        return segment_it->second;
    }

    void create_page(uint32_t tag, bool r, bool w, bool x) {
        if (exist_tag(tag)) return;
        Segment segment = 
        {
            .tag = tag,
            .data = std::vector<uint8_t>(alignment_),
            .r = r,
            .w = w,
            .x = x
        };
        segments_[tag] = std::move(segment);
    }

    Parser::SegmentInfo get_alligned_segment(const Parser::SegmentInfo &inp_segment_info) {
        uint32_t aligned_vaddr = inp_segment_info.vaddr & ~(alignment_ - 1);
        uint32_t offset = inp_segment_info.vaddr - aligned_vaddr;
        std::vector<uint8_t> padded_data(inp_segment_info.data.size() + offset, 0);

        std::copy(inp_segment_info.data.begin(), inp_segment_info.data.end(), padded_data.begin() + offset);
        Parser::SegmentInfo segment_info = 
        {
            .vaddr = aligned_vaddr,
            .memsz = offset + inp_segment_info.memsz,
            .filesz = offset + inp_segment_info.filesz,
            .data = std::move(padded_data),
            .r = inp_segment_info.r,
            .w = inp_segment_info.w,
            .x = inp_segment_info.x
        };
        return segment_info;
    }

public:
    MEM32() = default;

    bool exist_tag(uint32_t tag) const { return segments_.find(tag) != segments_.end(); }

    void add_segment(const Parser::SegmentInfo &inp_segment_info) override {
        Parser::SegmentInfo segment_info = get_alligned_segment(inp_segment_info);
    
        uint32_t start_tag = segment_info.vaddr / alignment_;
        uint32_t end_tag = start_tag + (segment_info.memsz + alignment_ - 1) / alignment_;
        for (uint32_t tag = start_tag; tag < end_tag; tag++) {
            if (exist_tag(tag)) throw SegmentOverlap(segment_info.vaddr, tag * alignment_);

            std::vector<uint8_t> page_data(alignment_, 0);

            size_t source_offset = (tag - start_tag) * alignment_;
            if (source_offset < segment_info.data.size()) {
                size_t bytes_to_copy = std::min(
                    static_cast<size_t>(alignment_), 
                    segment_info.data.size() - source_offset
                );
                std::copy(segment_info.data.begin() + source_offset, 
                          segment_info.data.begin() + source_offset + bytes_to_copy, 
                          page_data.begin());
            }

            Segment segment = {
                tag,
                std::move(page_data),
                segment_info.r, segment_info.w, segment_info.x
            };

            segments_[tag] = std::move(segment);
        }
    }

    void write32(uint64_t addr, uint32_t v) override {
        Segment &segment = get_seg(addr, /*rwx:*/ false, true, false);
        
        uint64_t offset = addr - segment.tag * alignment_;

        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(addr);
        }

        // Little-Endian Write
        segment.data[offset + 0] = static_cast<uint8_t>(v & 0xFF);         // Byte 0 (LSB)
        segment.data[offset + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);  // Byte 1
        segment.data[offset + 2] = static_cast<uint8_t>((v >> 16) & 0xFF); // Byte 2
        segment.data[offset + 3] = static_cast<uint8_t>((v >> 24) & 0xFF); // Byte 3 (MSB)
    }

    uint32_t read32(uint64_t addr) override {
        const Segment &segment = get_seg(static_cast<uint32_t>(addr), true, false, false);
        
        uint64_t offset = addr - segment.tag * alignment_;
    
        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(static_cast<uint32_t>(addr));
        }

        // Reconstruct 32-bit word (Little-Endian)
        return static_cast<uint32_t>(segment.data[offset + 0])      |
            (static_cast<uint32_t>(segment.data[offset + 1]) << 8)  |
            (static_cast<uint32_t>(segment.data[offset + 2]) << 16) |
            (static_cast<uint32_t>(segment.data[offset + 3]) << 24);
    }

    uint32_t read_instr32(uint64_t addr) override {
        const Segment& segment = get_seg(addr, /*rwx:*/ true, false, false);
        
        uint64_t offset = addr - segment.tag * alignment_;
        if (offset + 4 > segment.data.size()) {
            throw BoundaryFault(static_cast<uint32_t>(addr));
        }

        // Reconstruct 32-bit word (Little-Endian)
        return static_cast<uint32_t>(segment.data[offset + 0])      |
            (static_cast<uint32_t>(segment.data[offset + 1]) << 8)  |
            (static_cast<uint32_t>(segment.data[offset + 2]) << 16) |
            (static_cast<uint32_t>(segment.data[offset + 3]) << 24);
        }

    uint8_t read8(uint64_t addr) override {
        const Segment &segment = get_seg(static_cast<uint32_t>(addr), true, false, false);
        uint32_t offset = static_cast<uint32_t>(addr) - segment.tag * alignment_;

        if (offset >= segment.data.size()) {
            throw BoundaryFault(addr);
        }
        return segment.data[offset];
    }

    void write8(uint64_t addr, uint8_t v) override {
        Segment &segment = get_seg(static_cast<uint32_t>(addr), false, true, false);
        uint32_t offset = static_cast<uint32_t>(addr) - segment.tag * alignment_;

        if (offset >= segment.data.size()) {
            throw BoundaryFault(addr);
        }
        segment.data[offset] = v;
    }
    
    uint16_t read16(uint64_t addr) override {
        const Segment &segment = get_seg(static_cast<uint32_t>(addr), true, false, false);
        uint32_t offset = static_cast<uint32_t>(addr) - segment.tag * alignment_;

        if (offset + 2 > segment.data.size()) {
            throw BoundaryFault(addr);
        }

        return static_cast<uint16_t>(segment.data[offset + 0]) |
            static_cast<uint16_t>(segment.data[offset + 1] << 8);
    }

    void write16(uint64_t addr, uint16_t v) override {
        Segment &segment = get_seg(static_cast<uint32_t>(addr), false, true, false);
        uint32_t offset = static_cast<uint32_t>(addr) - segment.tag * alignment_;

        if (offset + 2 > segment.data.size()) {
            throw BoundaryFault(addr);
        }

        segment.data[offset + 0] = static_cast<uint8_t>(v & 0xFF);
        segment.data[offset + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);
    }

    void dump_segments() const {
        std::map<uint32_t, const Segment*> sorted_segments;
        for (const auto& [tag, seg] : segments_) {
            sorted_segments[tag] = &seg;
        }

        std::cout << "\n======================== MEMORY SEGMENT DUMP ========================\n";
        std::cout << std::format("{:<12} | {:<12} | {:<6} | {:<8} | {:<10}\n", 
                                "Start VAddr", "End VAddr", "Prot", "Tag", "Size(Hex)");
        std::cout << std::string(65, '-') << "\n";

        for (const auto& [tag, seg] : sorted_segments) {
            uint32_t start_vaddr = tag * alignment_;
            uint32_t end_vaddr = start_vaddr + static_cast<uint32_t>(seg->data.size());

            std::string prot = std::format("{}{}{}", 
                                        seg->r ? 'r' : '-', 
                                        seg->w ? 'w' : '-', 
                                        seg->x ? 'x' : '-');

            std::cout << std::format("0x{:08x}   0x{:08x}   {:<6}   0x{:05x}   0x{:x}\n", 
                                    start_vaddr, end_vaddr, prot, tag, seg->data.size());
            
            dump_hex_preview(seg->data);
        }
    std::cout << "=====================================================================\n" << std::endl;
}

    void dump_hex_preview(const std::vector<uint8_t>& data) const {
        if (data.empty()) return;
        std::cout << "  Preview: ";
        size_t preview_len = std::min<size_t>(data.size(), 16);
        for (size_t i = 0; i < preview_len; ++i) {
            std::cout << std::format("{:02x} ", data[i]);
        }
        if (data.size() > 16) std::cout << "...";
        std::cout << "\n\n";
    }
};

} // namespace rv