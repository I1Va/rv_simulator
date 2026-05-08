#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
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
    if (config->RomSize != 0) {
      state->mem->add_segment(
          makeSegment(config->RomStart, config->RomSize, true, false, true));
    }
    if (config->RamSize != 0) {
      state->mem->add_segment(
          makeSegment(config->RamStart, config->RamSize, true, true, false));
    }
  } catch (...) {
    return nullptr;
  }

  if (config->RomSize != 0) {
    state->cpu->set_pc(config->RomStart);
  } else {
    state->cpu->set_pc(0);
  }

  return state.release();
}

void rvm_modelDestroy(RVMState *state) { delete state; }

const RVMConfig *rvm_getModelConfig(const RVMState *state) {
  if (state == nullptr) return nullptr;
  return &state->config;
}

int rvm_executeInstr(RVMState *state) {
  if (state == nullptr || !state->cpu || !state->mem) return 1;
  try {
    rv::Instruction instruction =
        state->cpu->fetch_and_decode(state->cpu->pc(), *state->mem);
    state->cpu->execute(instruction, *state->mem);
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
  state->cpu->set_pc(newPC);
}

RVMRegT rvm_readXReg(const RVMState *state, RVMXReg reg) {
  if (state == nullptr || !state->cpu) return 0;
  return static_cast<RVMRegT>(state->cpu->read_reg(static_cast<uint8_t>(reg)));
}

void rvm_setXReg(RVMState *state, RVMXReg reg, RVMRegT value) {
  if (state == nullptr || !state->cpu) return;
  state->cpu->write_reg(static_cast<uint8_t>(reg), value);
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

void rvm_logMessage(const char *message) {
  if (message == nullptr) return;
  std::cout << message;
}

int rvm_queryCallbackSupportPresent() { return 0; }

} // extern "C"