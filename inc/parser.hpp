#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string_view>
#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <iomanip>

namespace rv 
{
 
class Parser {
public:
    struct SegmentInfo {
        uint32_t vaddr;
        uint32_t memsz;
        uint32_t filesz;
        std::vector<uint8_t> data;
        bool r, w, x;
    };

private:
    uint32_t entry_point_ = 0;
    std::vector<SegmentInfo> segments_;
    std::vector<std::pair<std::string, uint32_t>> section_names_;

    void read_section_names(std::ifstream& file, const Elf32_Ehdr& ehdr) {
        if (ehdr.e_shnum == 0) return;

        std::vector<Elf32_Shdr> shdrs(ehdr.e_shnum);
        file.seekg(ehdr.e_shoff);
        file.read(reinterpret_cast<char*>(shdrs.data()), ehdr.e_shnum * sizeof(Elf32_Shdr));

        const auto& strtab_shdr = shdrs[ehdr.e_shstrndx];
        std::vector<char> strtab(strtab_shdr.sh_size);
        file.seekg(strtab_shdr.sh_offset);
        file.read(strtab.data(), strtab_shdr.sh_size);

        for (const auto& shdr : shdrs) {
            std::string name = &strtab[shdr.sh_name];
            if (!name.empty() && shdr.sh_addr != 0) {
                section_names_.push_back({name, shdr.sh_addr});
            }
        }
    }

public:
    Parser(const std::string_view elf_path) {
        std::ifstream file(elf_path.data(), std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open ELF file: " + std::string(elf_path));
        }

        Elf32_Ehdr ehdr;
        file.read(reinterpret_cast<char*>(&ehdr), sizeof(Elf32_Ehdr));

        if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
            throw std::runtime_error("Not a valid ELF file");
        }
        
        if (ehdr.e_machine != EM_RISCV) {
            throw std::runtime_error("Not a RISC-V binary");
        }
        
        entry_point_ = ehdr.e_entry;

        read_section_names(file, ehdr);
    
        std::vector<Elf32_Phdr> phdrs(ehdr.e_phnum);
        file.seekg(ehdr.e_phoff);
        file.read(reinterpret_cast<char*>(phdrs.data()), ehdr.e_phnum * sizeof(Elf32_Phdr));

        for (const auto& phdr : phdrs) {
            if (phdr.p_type == PT_LOAD) {
                SegmentInfo seg;
                seg.vaddr = phdr.p_vaddr;
                seg.memsz = phdr.p_memsz;
                seg.filesz = phdr.p_filesz;
                
                seg.r = phdr.p_flags & PF_R;
                seg.w = phdr.p_flags & PF_W;
                seg.x = phdr.p_flags & PF_X;

                seg.data.resize(phdr.p_filesz);
                file.seekg(phdr.p_offset);
                file.read(reinterpret_cast<char*>(seg.data.data()), phdr.p_filesz);

                segments_.push_back(std::move(seg));
            }
        }
    }

    uint32_t get_entry_point() const { return entry_point_; }
    const std::vector<SegmentInfo>& get_segments() const { return segments_; }

    void dump() const {
        std::cout << "\n=== ELF Parser Dump ===" << std::endl;
        // ... (your existing header code) ...
        std::cout << std::left << std::setw(12) << "VirtAddr" 
                  << std::setw(10) << "MemSize" 
                  << std::setw(8) << "Flags" 
                  << std::setw(20) << "Mapped Sections"
                  << "Data Preview" << std::endl;
        std::cout << "-----------------------------------------------------------------------" << std::endl;

        for (const auto& seg : segments_) {
            // Find which sections fall inside this segment's range
            std::string contained_sections = "";
            for (const auto& [name, addr] : section_names_) {
                if (addr >= seg.vaddr && addr < (seg.vaddr + seg.memsz)) {
                    contained_sections += name + " ";
                }
            }

            std::string flags = (seg.r ? "R" : "-");
            flags += (seg.w ? "W" : "-");
            flags += (seg.x ? "X" : "-");

            std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << seg.vaddr << "  "
                      << "0x" << std::setw(6) << std::setfill('0') << seg.memsz << "  "
                      << std::setw(6) << std::setfill(' ') << flags << "  "
                      << std::setw(20) << contained_sections
                      << std::hex << std::setw(2) << static_cast<int>(seg.data[0]) << " " 
                      << std::setw(2) << static_cast<int>(seg.data[1]) << "..." 
                      << std::dec << std::endl;
        }
    }

};

} // namespace rv