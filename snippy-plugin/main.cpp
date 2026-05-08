#include <cstdint>
#include <cctype>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <memory>
#include <algorithm>
#include <vector>

#include "RVM.hpp"
#include "cpu.hpp"
#include "decoder.hpp"
#include "memory.hpp"
#include "parser.hpp"

struct RVMState {
  RVMConfig config{};
  std::unique_ptr<rv::MEM32> mem;
  std::unique_ptr<rv::CPU_RV32I> cpu;
  bool uses_fallback_memory_map = false;
};

static rv::Parser::SegmentInfo makeSegment(uint64_t start, uint64_t size,
                                           bool r, bool w, bool x) {
  rv::Parser::SegmentInfo seg{};
  seg.vaddr = static_cast<uint32_t>(start);
  seg.memsz = static_cast<uint32_t>(size);
  seg.filesz = static_cast<uint32_t>(size);
  seg.data.assign(static_cast<size_t>(size), 0);
  seg.r = r;
  seg.w = w;
  seg.x = x;
  return seg;
}

static bool contiguousRegions(uint64_t a_start, uint64_t a_sz, uint64_t b_start,
                              uint64_t b_sz) {
  if (a_sz == 0 || b_sz == 0)
    return false;
  return a_start + a_sz == b_start || b_start + b_sz == a_start;
}

static uint64_t regionLow(uint64_t s1, uint64_t s2) { return std::min(s1, s2); }

static uint64_t regionHighEnd(uint64_t s1, uint64_t sz1, uint64_t s2,
                              uint64_t sz2) {
  return std::max(s1 + sz1, s2 + sz2);
}

static bool instruction_trace_enabled() {
  const char *v = std::getenv("SNIPPY_RV32_TRACE");
  if (v == nullptr)
    return true;
  return v[0] != '\0' && v[0] != '0';
}

static constexpr uint64_t kFallbackRomStart = 0x1000;
static constexpr uint64_t kFallbackRomSpan = 0x20000;

static bool rvm_config_memory_plausible(uint64_t rom_st, uint64_t rom_sz,
                                         uint64_t ram_st, uint64_t ram_sz) {
  if (rom_sz == 0 && ram_sz == 0)
    return false;
  if (rom_sz != 0) {
    if ((rom_st & 0xFFFu) != 0 || rom_st < kFallbackRomStart || rom_st > 0xFFFF0000u)
      return false;
    if (rom_sz < 0x1000u || rom_sz > 0x10000000u)
      return false;
  }
  if (ram_sz != 0) {
    if ((ram_st & 0xFFFu) != 0 || ram_st > 0xFFFF0000u)
      return false;
    if (ram_sz < 0x1000u || ram_sz > 0x10000000u)
      return false;
  }
  return true;
}

static uint64_t resolveSnippetPC(const RVMState *state, uint64_t pc) {
  if (state == nullptr)
    return pc;
  if (pc != 0)
    return pc;
  if (state->uses_fallback_memory_map)
    return kFallbackRomStart;
  if (state->config.RomSize > 0 &&
      rvm_config_memory_plausible(state->config.RomStart, state->config.RomSize,
                                  state->config.RamStart, state->config.RamSize))
    return state->config.RomStart;
  return kFallbackRomStart;
}

extern "C" {

RVMState *rvm_modelCreate(const RVMConfig *config) {
  if (config == nullptr) return nullptr;

  rv::Config simConfig{};
  simConfig.logs_disabled = true;
  simConfig.isa = "rv32i";
  auto state = std::make_unique<RVMState>();
  state->config = *config;
  state->mem = std::make_unique<rv::MEM32>();
  state->cpu = std::make_unique<rv::CPU_RV32I>(simConfig);

  try {
    const uint64_t rom_st = config->RomStart;
    const uint64_t rom_sz = config->RomSize;
    const uint64_t ram_st = config->RamStart;
    const uint64_t ram_sz = config->RamSize;

    const bool plausible =
        rvm_config_memory_plausible(rom_st, rom_sz, ram_st, ram_sz);

    if (!plausible) {
      state->uses_fallback_memory_map = true;
      state->mem->add_segment(
          makeSegment(kFallbackRomStart, kFallbackRomSpan, true, true, true));
    } else if (rom_sz != 0 && ram_sz != 0 &&
               contiguousRegions(rom_st, rom_sz, ram_st, ram_sz)) {
      const uint64_t low = regionLow(rom_st, ram_st);
      const uint64_t span = regionHighEnd(rom_st, rom_sz, ram_st, ram_sz) - low;
      state->mem->add_segment(makeSegment(low, span, true, true, true));
    } else {
      if (rom_sz != 0) {
        state->mem->add_segment(
            makeSegment(rom_st, rom_sz, true, true, true));
      }
      if (ram_sz != 0) {
        state->mem->add_segment(
            makeSegment(ram_st, ram_sz, true, true, true));
      }
    }
  } catch (...) {
    return nullptr;
  }

  if (state->uses_fallback_memory_map) {
    state->cpu->set_pc(kFallbackRomStart);
  } else if (config->RomSize != 0) {
    state->cpu->set_pc(config->RomStart);
  } else if (config->RamSize != 0) {
    state->cpu->set_pc(config->RamStart);
  } else {
    state->cpu->set_pc(0);
  }

  return state.release();
}

void rvm_modelDestroy(RVMState *state) {
  if (state != nullptr && instruction_trace_enabled()) {
    static const char kTraceEnd[] =
        "#===Snippy RV32 plugin trace end===\n";
    static_cast<void>(
        write(STDOUT_FILENO, kTraceEnd, sizeof(kTraceEnd) - 1));
  }
  delete state;
}

const RVMConfig *rvm_getModelConfig(const RVMState *state) {
  if (state == nullptr) return nullptr;
  return &state->config;
}

void rvm_reservedAfterGetModelConfig(RVMState *state) {
  (void)state;
}

void rvm_notifyExecutionModeBeforeSetPc(RVMState *state, uint64_t mode) {
  (void)mode;
  if (state == nullptr || !state->cpu)
    return;
  if (state->cpu->pc() == 0)
    state->cpu->set_pc(resolveSnippetPC(state, 0));
}

int rvm_executeInstr(RVMState *state) {
  if (state == nullptr || !state->cpu || !state->mem) return 1;
  try {
    if (state->cpu->pc() == 0)
      state->cpu->set_pc(resolveSnippetPC(state, 0));
    const uint32_t pc0 = static_cast<uint32_t>(state->cpu->pc());
    const bool trace = instruction_trace_enabled();
    uint32_t word0 = 0;
    if (trace)
      word0 = state->mem->read_instr32(pc0);
    rv::Instruction instruction =
        state->cpu->fetch_and_decode(pc0, *state->mem);
    state->cpu->execute(instruction, *state->mem);
    if (trace) {
      char buf[80];
      const int n = std::snprintf(buf, sizeof(buf),
                                 "[snippy-rv32] 0x%08" PRIx32 "  0x%08" PRIx32 "\n",
                                 pc0, word0);
      if (n > 0)
        static_cast<void>(write(STDOUT_FILENO, buf, static_cast<size_t>(n)));
    }
    return 0;
  } catch (...) {
    return 1;
  }
}

void rvm_readMem(const RVMState *state, uint64_t addr, size_t count, char *data) {
  if (state == nullptr || !state->mem || data == nullptr) return;
  for (size_t i = 0; i < count; ++i) {
    data[i] = static_cast<char>(state->mem->read8(addr + i));
  }
}

void rvm_writeMem(RVMState *state, uint64_t addr, size_t count,
                  const char *data) {
  if (state == nullptr || !state->mem || data == nullptr) return;
  for (size_t i = 0; i < count; ++i) {
    state->mem->write8(addr + i, static_cast<uint8_t>(data[i]));
  }
}

uint64_t rvm_readPC(const RVMState *state) {
  if (state == nullptr || !state->cpu) return 0;
  return state->cpu->pc();
}

void rvm_setPC(RVMState *state, uint64_t newPC) {
  if (state == nullptr || !state->cpu) return;
  state->cpu->set_pc(resolveSnippetPC(state, newPC));
}

RVMRegT rvm_readXReg(const RVMState *state, RVMXReg reg) {
  if (state == nullptr || !state->cpu) return 0;
  
  uint32_t reg_idx = static_cast<uint32_t>(reg);
  
  if (reg_idx == 32) {
      return static_cast<RVMRegT>(state->cpu->pc());
  }
  
  if (reg_idx > 31) return 0; 
  
  return static_cast<RVMRegT>(state->cpu->read_reg(static_cast<uint8_t>(reg_idx)));
}

void rvm_setXReg(RVMState *state, RVMXReg reg, RVMRegT value) {
  if (state == nullptr || !state->cpu) return;
  
  uint32_t reg_idx = static_cast<uint32_t>(reg);
  
  if (reg_idx == 32) {
      state->cpu->set_pc(resolveSnippetPC(state, value));
      return;
  }
  
  if (reg_idx > 31) return;
  
  state->cpu->write_reg(static_cast<uint8_t>(reg_idx), value);
}

RVMRegT rvm_readFReg(const RVMState *, RVMFReg) { return 0; }
void rvm_setFReg(RVMState *, RVMFReg, RVMRegT) {}

RVMRegT rvm_readCSRReg(const RVMState *state, unsigned reg) {
  if (state == nullptr) return 0;
  switch (reg) {
    case RVM_CSR_MISA:
      return static_cast<RVMRegT>(state->config.MisaExt);
    default:
      return 0;
  }
}

void rvm_setCSRReg(RVMState *, unsigned, RVMRegT) {}

int rvm_readVReg(const RVMState *, RVMVReg, char *, size_t) { return 1; }
int rvm_setVReg(RVMState *, RVMVReg, const char *, size_t) { return 1; }

void rvm_logMessage(const RVMState *state, const char *message) {
  (void)state;
  if (message == nullptr)
    return;
  const size_t kMaxChunk = 1u << 20;
  const size_t n = strnlen(message, kMaxChunk);
  std::vector<char> out;
  out.reserve(n + 1);
  size_t kept = 0;
  for (size_t i = 0; i < n; ++i) {
    const unsigned char c = static_cast<unsigned char>(message[i]);
    if (c == '\n' || c == '\r' || c == '\t') {
      out.push_back(static_cast<char>(c));
      ++kept;
    } else if (std::isprint(c) != 0) {
      out.push_back(static_cast<char>(c));
      ++kept;
    }
  }
  if (kept < n)
    return;
  if (out.empty())
    return;
  if (out.back() != '\n')
    out.push_back('\n');
  static_cast<void>(write(STDOUT_FILENO, out.data(), out.size()));
}

int rvm_queryCallbackSupportPresent() { return 0; }

} // extern "C"
