#ifndef IR_HH
#define IR_HH

#include "instrusive_list.hh"

namespace jj_ir {

class IRBuilder;
class Instr;

class BasicBlock final : public INode<BasicBlock> {
    IntrusiveList<Instr> m_instrs;
    friend IRBuilder;
};

class IRBuilder final {
    //
};

}  // namespace jj_ir

#endif  // IR_HH