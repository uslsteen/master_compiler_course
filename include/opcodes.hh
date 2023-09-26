#ifndef OPCODES_HH
#define OPCODES_HH

#include <cstdint>

namespace jj_ir {

enum class Opcode : uint16_t {
    NONE,
    ADD, 
    SUB,
    MUL,
    DIV
    //! NOTE: etc
};

}


#endif  // OPCODES_HH