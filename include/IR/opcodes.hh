#ifndef OPCODES_HH
#define OPCODES_HH

#include <cstdint>
#include <set>

namespace jj_vm::ir {

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
    SHR,
    XOR,
    EQ,
    LE,
    GE,
    /* Unary instructions */
    NEG,
    /* Contorl flow */
    RET,
    BRANCH,
    IF,
    /* Other */
    PHI,
    CAST,
    CONST,
    PARAM,
    CALL,
    /* Checks Elimiation */
    BOUNDS_CHECK,
    NULL_CHECK
};
}  // namespace jj_vm::ir

#endif  // OPCODES_HH
