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
    using GraphTy = jj_vm::graph::BBGraph;
    using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;

private:
    std::vector<std::reference_wrapper<jj_vm::ir::Instr>> m_instrs{};
    //
    std::set<OpcodeTy> unfoldable_set{OpcodeTy::NONE,   OpcodeTy::RET,
                                      OpcodeTy::BRANCH, OpcodeTy::IF,
                                      OpcodeTy::PHI,    OpcodeTy::CONST};

    std::set<OpcodeTy> bin_op_set{OpcodeTy::ADD, OpcodeTy::SUB, OpcodeTy::MUL,
                                  OpcodeTy::DIV, OpcodeTy::SHR, OpcodeTy::XOR,
                                  OpcodeTy::EQ,  OpcodeTy::LE,  OpcodeTy::GE};

public:
    void run(jj_vm::ir::Function* func) override {
        visit_func(func);
        optimize();
    }

    void visit_instr(jj_vm::ir::Instr& instr) override {
        auto find_res = unfoldable_set.find(instr.opcode());

        if (find_res == unfoldable_set.end()) m_instrs.push_back(instr);
    }

    bool is_really_need_folding(jj_vm::ir::Instr& instr) {
        for (auto* input : instr.inputs()) {
            auto* cur_input = static_cast<jj_vm::ir::Instr*>(input);
            if (cur_input->opcode() != OpcodeTy::CONST) return false;
        }
        return true;
    }

    template <typename Type>
    std::unique_ptr<jj_vm::ir::Instr> eval_binary_operation(
        const jj_vm::ir::Instr* l_instr, const jj_vm::ir::Instr* r_instr,
        OpcodeTy opc) {
        const auto* lhs =
            static_cast<const jj_vm::ir::Constant<Type>*>(l_instr);
        const auto* rhs =
            static_cast<const jj_vm::ir::Constant<Type>*>(r_instr);

        Type lval = lhs->val(), rval = rhs->val(), result{};
        //
        switch (opc) {
            case OpcodeTy::ADD:
                result = lval + rval;
                break;
            case OpcodeTy::SUB:
                result = lval - rval;
                break;
            case OpcodeTy::MUL:
                result = lval * rval;
                break;
            case OpcodeTy::DIV: {
                if (rval == 0)
                    assert(false &&
                           "Error: division by zero is not yet supported");
                else {
                    result = lval / rval;
                    break;
                }
            }
            case OpcodeTy::SHR: {
                if (rval >= std::numeric_limits<Type>::digits || rval < 0)
                    assert(false &&
                           "Error: incorrect shift value in signed shift right "
                           "operation");
                else {
                    result = lval >> rval;
                    break;
                }
            }
            case OpcodeTy::XOR:
                result = lval ^ rval;
                break;
            case OpcodeTy::EQ:
                result = lval == rval;
                break;
            case OpcodeTy::LE:
                result = lval < rval;
                break;
            case OpcodeTy::GE:
                result = lval > rval;
                break;
            default: {
                //! NOTE: I don't want to process all issues with div ....
                assert(false &&
                       "Error: unsupported evaluation binary operation");
            }
        }

        return std::make_unique<jj_vm::ir::Constant<Type>>(result);
    }

    std::unique_ptr<jj_vm::ir::Instr> fold_binary_operation(
        jj_vm::ir::Instr& instr) {
        auto* lhs = static_cast<const jj_vm::ir::Instr*>(instr.get_input(0));
        auto* rhs = static_cast<const jj_vm::ir::Instr*>(instr.get_input(1));
        //
        auto type = instr.type();
        auto opcode = instr.opcode();

        switch (type) {
            case jj_vm::ir::TypeId::I1:
                return eval_binary_operation<bool>(lhs, rhs, opcode);
            case jj_vm::ir::TypeId::I8:
                return eval_binary_operation<std::int8_t>(lhs, rhs, opcode);
            case jj_vm::ir::TypeId::I16:
                return eval_binary_operation<std::int16_t>(lhs, rhs, opcode);
            case jj_vm::ir::TypeId::I32:
                return eval_binary_operation<std::int32_t>(lhs, rhs, opcode);
            case jj_vm::ir::TypeId::I64:
                return eval_binary_operation<std::int64_t>(lhs, rhs, opcode);
            case jj_vm::ir::TypeId::NONE:
                assert(false && "Error: uknown folding type");
        }
    }

    std::unique_ptr<jj_vm::ir::Instr> fold_operation(jj_vm::ir::Instr& instr) {
        auto is_bin_op = bin_op_set.find(instr.opcode()) != bin_op_set.end();

        //! TODO: suuport unary, cast operations folding
        if (is_bin_op) return fold_binary_operation(instr);
    }

    void optimize() {
        for (auto ref_instr : m_instrs) {
            auto& instr = ref_instr.get();

            if (!is_really_need_folding(instr)) continue;

            auto optimized_instr = fold_operation(instr);
            auto* parent = instr.parent();
            //
            instr.clean_inputs();
            parent->replace_instr(&instr, optimized_instr.release());
        }
    }
};
}  // namespace jj_vm::passes
