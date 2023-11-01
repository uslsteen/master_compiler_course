#ifndef OPCODES_HH
#define OPCODES_HH

#include <cstdint>

namespace jj_ir {

enum class InstType : uint8_t {

};

enum class Opcode : uint16_t {
    /*  Default none value  */
    NONE,
    /*  Binary instructions */
    ADD,
    SUB,
    MUL,
    DIV,
    LE,
    GE,
    /* Contorl flow */
    RET,
    BRANCH,
    IF,
    /* Other */
    PHI,
    CAST,
    CONST,
    //! NOTE: etc
};

}  // namespace jj_ir

#endif  // OPCODES_HH