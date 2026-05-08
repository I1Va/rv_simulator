#pragma once
#include "RVM.hpp"

#ifdef __cplusplus
extern "C" {
namespace rvm {
#endif // __cplusplus

/// Must match llvm-snippy RVM interface v29: 21 pointers (168 bytes). Older
/// snapshots (e.g. notes/rvdash with v9) used 18 entries — that layout will
/// crash under current Snippy because offsets for readCSR / logMessage shift.
struct RVM_FunctionPointers {
  rvm_modelCreate_t modelCreate;
  rvm_modelDestroy_t modelDestroy;

  rvm_getModelConfig_t getModelConfig;

  rvm_reservedAfterGetModelConfig_t reservedAfterGetModelConfig;

  rvm_executeInstr_t executeInstr;

  rvm_readMem_t readMem;
  rvm_writeMem_t writeMem;

  rvm_notifyExecutionMode_t notifyExecutionModeBeforeSetPc;
  rvm_setPC_t setPCStopModeAlias;

  rvm_readPC_t readPC;
  rvm_setPC_t setPC;

  rvm_readXReg_t readXReg;
  rvm_setXReg_t setXReg;

  rvm_readFReg_t readFReg;
  rvm_setFReg_t setFReg;

  rvm_readCSRReg_t readCSRReg;
  rvm_setCSRReg_t setCSRReg;

  rvm_readVReg_t readVReg;
  rvm_setVReg_t setVReg;

  rvm_logMessage_t logMessage;
  rvm_queryCallbackSupportPresent_t queryCallbackSupportPresent;
};

#ifdef __cplusplus
}
} // namespace rvm
#endif // __cplusplus
