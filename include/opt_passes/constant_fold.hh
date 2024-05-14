#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
//
#include "pass_manager.hh"
#include "pass_visitor.hh"

namespace jj_vm::passes {

class ConstantFold : Pass, PassVisitor {
public:
    using OpcodeTy = jj_vm::ir::Opcode;

private:
    std::vector<std::reference_wrapper<jj_vm::ir::Instr>> m_instrs{};
    //
    std::set<OpcodeTy> unfoldable{OpcodeTy::NONE,   OpcodeTy::RET,
                                  OpcodeTy::BRANCH, OpcodeTy::IF,
                                  OpcodeTy::PHI,    OpcodeTy::CONST};

public:
    void run(jj_vm::ir::Function* func) override {
        visit_func(func);
        optimize();
    }

    void visit_instr(jj_vm::ir::Instr& instr) override {
        auto find_res = unfoldable.find(instr.opcode());

        if (find_res == unfoldable.end()) m_instrs.push_back(instr);
    }

    bool is_really_need_folding(jj_vm::ir::Instr& instr) {
        for (auto* input : instr.inputs()) {
            auto* cur_input = static_cast<jj_vm::ir::Instr*>(input);
            if (cur_input->opcode() != OpcodeTy::CONST) return false;
        }
        return true;
    }

    template <typename BinOpTy>
    void fold_binary_operation(jj_vm::ir::Instr& instr, BinOpTy op) {
        auto* lhs = instr.get_input(0);
        auto* rhs = instr.get_input(1);

        auto* bb = instr.parent();
        auto type = instr.type();
    }

        void optimize() {
        for (auto ref_instr : m_instrs) {
            auto& instr = ref_instr.get();

            if (!is_really_need_folding(instr)) continue;
        }
    }
};
}  // namespace jj_vm::passes
