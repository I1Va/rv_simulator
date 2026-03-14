#pragma once

namespace rv
{

class IDecoder {
    // decoder interface
    // can fetch and return Instruction Interface entity

    // Instruction decode(int addr)
};

class RV32Decoder : IDecoder {
    // RV32I realization
    // depends on RV32 extenstions 
    // (for example, if C extension is not included and 2 byte instrcution is met, it will return exception)    
};

} // namespace rv

