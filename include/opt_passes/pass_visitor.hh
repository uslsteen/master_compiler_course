#pragma once

#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
//
#include "graph/dfs.hh"

namespace jj_vm::passes {

class PassVisitor {
public:
    virtual void visit_func(jj_vm::ir::Function* func) {
        auto graph = func->bb_graph();
        auto rpo = jj_vm::graph::deep_first_search_reverse_postoder(graph);
        //
        for (auto* bb : rpo) visit_block(bb);
    }

    //
    virtual void visit_block(jj_vm::ir::BasicBlock* bb) {
        auto& block = *bb;
        for (auto& instr : block) {
            instr.opcode();
            visit_instr(instr);
        }
    }

    virtual void visit_instr(jj_vm::ir::Instr& instr) {
        //! NOTE: should be overrided
    }
};

}  // namespace jj_vm::passes
