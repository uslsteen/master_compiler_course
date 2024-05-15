#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "pass_manager.hh"
#include "pass_visitor.hh"
//

namespace jj_vm::passes {
class Peephole : Pass, PassVisitor {
public:
    using GraphTy = jj_vm::graph::BBGraph;
    using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;

private:
    std::vector<std::reference_wrapper<jj_vm::ir::Instr>> m_instrs{};

public:
    void run(jj_vm::ir::Function* func) override {
        visit_func(func);
        optimize();
    }

    void visit_instr(jj_vm::ir::Instr& instr) override {
        switch (instr.opcode()) {
            case jj_vm::ir::Opcode::MUL:
            case jj_vm::ir::Opcode::SHR:
            case jj_vm::ir::Opcode::XOR: {
                m_instrs.push_back(instr);
                break;
            }
            default: {
                break;
            }
        }
    }

    bool is_really_need_peephole(jj_vm::ir::Instr& instr) {
        return !instr.users().empty();
    }

    void optimize() {
        for (auto ref_instr : m_instrs) {
            auto& instr = ref_instr.get();

            if (!is_really_need_peephole(instr)) continue;

            switch (instr.opcode()) {
                case jj_vm::ir::Opcode::MUL: {
                    process_mul(instr);
                    break;
                }
                case jj_vm::ir::Opcode::SHR: {
                    process_shr(instr);
                    break;
                }
                case jj_vm::ir::Opcode::XOR: {
                    process_xor(instr);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    template <typename ConstTy, ConstTy const_value>
    bool compare_val(const jj_vm::ir::Instr* instr) {
        return static_cast<const jj_vm::ir::Constant<ConstTy>*>(instr)->val() ==
               const_value;
    }

    template <typename ConstTy>
    bool compare_val(const jj_vm::ir::Instr* lhs, const jj_vm::ir::Instr* rhs) {
        return static_cast<const jj_vm::ir::Constant<ConstTy>*>(lhs)->val() ==
               static_cast<const jj_vm::ir::Constant<ConstTy>*>(rhs)->val();
    }

    template <int const_value>
    bool check_const_val(const jj_vm::ir::Instr* instr) {
        switch (instr->type()) {
            case jj_vm::ir::TypeId::I1:
                return compare_val<bool, static_cast<bool>(const_value)>(instr);
            case jj_vm::ir::TypeId::I8:
                return compare_val<int8_t, int8_t{const_value}>(instr);
            case jj_vm::ir::TypeId::I16:
                return compare_val<int16_t, int16_t{const_value}>(instr);
            case jj_vm::ir::TypeId::I32:
                return compare_val<int32_t, int32_t{const_value}>(instr);
            case jj_vm::ir::TypeId::I64:
                return compare_val<int64_t, int64_t{const_value}>(instr);
            default: {
                assert(false && "Error: unsupported constant type in peephole");
                return false;
            }
        }
    }

    bool compare_const_val(const jj_vm::ir::Instr* lhs,
                           const jj_vm::ir::Instr* rhs) {
        bool same_type = lhs->type() == rhs->type();
        if (!same_type) return false;

        switch (lhs->type()) {
            case jj_vm::ir::TypeId::I1:
                return compare_val<bool>(lhs, rhs);
            case jj_vm::ir::TypeId::I8:
                return compare_val<int8_t>(lhs, rhs);
            case jj_vm::ir::TypeId::I16:
                return compare_val<int16_t>(lhs, rhs);
            case jj_vm::ir::TypeId::I32:
                return compare_val<int32_t>(lhs, rhs);
            case jj_vm::ir::TypeId::I64:
                return compare_val<int64_t>(lhs, rhs);
            default: {
                assert(false && "Error: unsupported constant type in peephole");
                return false;
            }
        }
    }

    void process_mul(jj_vm::ir::Instr& instr) {
        auto* lval = instr.get_input(0);
        auto* rval = instr.get_input(1);

        // Pattern 1:
        // MUL v0, 1 -> v0
        auto* const_instr = static_cast<jj_vm::ir::Instr*>(rval);
        if (const_instr->opcode() == jj_vm::ir::Opcode::CONST &&
            check_const_val<1>(const_instr)) {
            lval->replace_users(instr);
            jj_vm::ir::erase(&instr);
        }
    }

    //
    void process_shr(jj_vm::ir::Instr& instr) {
        auto* lval = instr.get_input(0);
        auto* rval = instr.get_input(1);

        // Pattern 1, zero shift
        // SHR v0, 1 -> v0
        auto* const_instr = static_cast<jj_vm::ir::Instr*>(rval);
        if (const_instr->opcode() == jj_vm::ir::Opcode::CONST &&
            check_const_val<0>(const_instr)) {
            lval->replace_users(instr);
            jj_vm::ir::erase(&instr);
        }

        // Pattern 2, commulative two sequantially shify
        // v1 = SHR v0, 1
        // v2 = SHR v1, 2
        // -->
        // const = add 1, 2
        // v2 = SHR v0, const

        //! TODO: implementation in progress
    }

    //
    void process_xor(jj_vm::ir::Instr& instr) {
        auto* lval = instr.get_input(0);
        auto* rval = instr.get_input(1);

        // Pattern 1:
        // XOR v0, 0 -> v0
        auto* const_instr = static_cast<jj_vm::ir::Instr*>(rval);
        if (const_instr->opcode() == jj_vm::ir::Opcode::CONST &&
            check_const_val<0>(const_instr)) {
            lval->replace_users(instr);
            jj_vm::ir::erase(&instr);
        }

        // Pattern 2, same registers
        // v0 = i64 1
        // XOR v0, v0 -> 0

        // Pattern 3, same constant values
        // v0 = i64 1
        // v1 = i64 1
        // XOR v0, v1 -> 0

        auto* lval_instr = static_cast<jj_vm::ir::Instr*>(lval);
        auto* rval_instr = static_cast<jj_vm::ir::Instr*>(rval);
        bool both_const = lval_instr->opcode() == jj_vm::ir::Opcode::CONST &&
                          rval_instr->opcode() == jj_vm::ir::Opcode::CONST;
        //
        bool is_equal_vals =
            both_const && compare_const_val(lval_instr, rval_instr);

        if (lval == rval || is_equal_vals) {
            std::unique_ptr<jj_vm::ir::Instr> const_zero{
                new jj_vm::ir::Constant<int64_t>{0}};
            instr.parent()->replace_instr(&instr, const_zero.release());
        }
    }
};
}  // namespace jj_vm::passes
