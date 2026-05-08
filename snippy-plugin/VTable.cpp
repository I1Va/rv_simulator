#include "VTable.hpp"

#include <cstdint>

extern "C" {

uint32_t RVMAPI_VERSION_SYMBOL = RVMAPI_CURRENT_INTERFACE_VERSION;

} // extern "C"

extern const rvm::RVM_FunctionPointers RVMAPI_ENTRY_POINT_SYMBOL = {
    .modelCreate = &rvm_modelCreate,
    .modelDestroy = &rvm_modelDestroy,

    .getModelConfig = &rvm_getModelConfig,

    .reservedAfterGetModelConfig = &rvm_reservedAfterGetModelConfig,

    .executeInstr = &rvm_executeInstr,

    .readMem = &rvm_readMem,
    .writeMem = &rvm_writeMem,

    .notifyExecutionModeBeforeSetPc = &rvm_notifyExecutionModeBeforeSetPc,
    .setPCStopModeAlias = &rvm_setPC,

    .readPC = &rvm_readPC,
    .setPC = &rvm_setPC,

    .readXReg = &rvm_readXReg,
    .setXReg = &rvm_setXReg,

    .readFReg = &rvm_readFReg,
    .setFReg = &rvm_setFReg,

    .readCSRReg = &rvm_readCSRReg,
    .setCSRReg = &rvm_setCSRReg,

    .readVReg = &rvm_readVReg,
    .setVReg = &rvm_setVReg,

    .logMessage = &rvm_logMessage,
    .queryCallbackSupportPresent = &rvm_queryCallbackSupportPresent,
};
