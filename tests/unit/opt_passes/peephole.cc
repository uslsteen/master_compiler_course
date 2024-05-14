#include <cstdint>

#include "../graph/builder.hh"
//
#include "opt_passes/peephole.hh"

namespace jj_vm::testing {
class PeepholeTestBuilder : public TestBuilder {
protected:
    jj_vm::passes::Peephole m_pass;
    jj_vm::ir::IRBuilder m_builder{};
};

TEST_F(PeepholeTestBuilder, ADD) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    /*
        TEST ADD
    */
    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(1);
    auto *another_val = m_builder.create<jj_vm::ir::ConstI64>(42);
    auto *mul = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::MUL,
                                                      lval, rval);
    auto *add = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD,
                                                      mul, another_val);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 4);
    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::ADD);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);
    //
    const auto &last_operation =
        static_cast<const jj_vm::ir::BinInstr &>(last_instr);
    ASSERT_EQ(last_operation.lhs(), lval);
    ASSERT_EQ(last_operation.rhs(), another_val);
}

TEST_F(PeepholeTestBuilder, SHR1) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(0);
    auto *another_val = m_builder.create<jj_vm::ir::ConstI64>(42);
    auto *no_shift = m_builder.create<jj_vm::ir::BinInstr>(
        jj_vm::ir::Opcode::SHR, lval, rval);
    auto *add = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD,
                                                      no_shift, another_val);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 4);
    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::ADD);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);
    //
    const auto &last_operation =
        static_cast<const jj_vm::ir::BinInstr &>(last_instr);
    ASSERT_EQ(last_operation.lhs(), lval);
    ASSERT_EQ(last_operation.rhs(), another_val);
}

TEST_F(PeepholeTestBuilder, SHR2) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *fst_shift = m_builder.create<jj_vm::ir::ConstI64>(16);
    auto *snd_shift = m_builder.create<jj_vm::ir::ConstI64>(15);
}

TEST_F(PeepholeTestBuilder, XOR1) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(0);
    auto *another_val = m_builder.create<jj_vm::ir::ConstI64>(42);
    auto *no_xor = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::XOR,
                                                         lval, rval);
    auto *add = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD,
                                                      no_xor, another_val);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 4);
    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::ADD);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);
    //
    const auto &last_operation =
        static_cast<const jj_vm::ir::BinInstr &>(last_instr);
    ASSERT_EQ(last_operation.lhs(), lval);
    ASSERT_EQ(last_operation.rhs(), another_val);
}

TEST_F(PeepholeTestBuilder, XOR2) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *val = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *another_val = m_builder.create<jj_vm::ir::ConstI64>(42);
    auto *zero_xor =
        m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::XOR, val, val);
    auto *add = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD,
                                                      zero_xor, another_val);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 4);
    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::ADD);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);
    //
    const auto &last_operation =
        static_cast<const jj_vm::ir::BinInstr &>(last_instr);

    const auto *lhs_operation =
        static_cast<const jj_vm::ir::Instr *>(last_operation.lhs());
    ASSERT_EQ(lhs_operation->opcode(), jj_vm::ir::Opcode::CONST);
    ASSERT_EQ(lhs_operation->type(), jj_vm::ir::TypeId::I64);

    const auto *some_constant =
        static_cast<const jj_vm::ir::ConstI64*>(lhs_operation);
    ASSERT_EQ(some_constant->val(), 0);
    //
    ASSERT_EQ(last_operation.rhs(), another_val);
}

TEST_F(PeepholeTestBuilder, XOR3) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *another_val = m_builder.create<jj_vm::ir::ConstI64>(42);
    auto *zero_xor = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::XOR,
                                                         lval, rval);
    auto *add = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD,
                                                      zero_xor, another_val);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 5);
    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::ADD);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);
    //
    const auto &last_operation =
        static_cast<const jj_vm::ir::BinInstr &>(last_instr);

    const auto *lhs_operation =
        static_cast<const jj_vm::ir::Instr *>(last_operation.lhs());
    ASSERT_EQ(lhs_operation->opcode(), jj_vm::ir::Opcode::CONST);
    ASSERT_EQ(lhs_operation->type(), jj_vm::ir::TypeId::I64);

    const auto *some_constant =
        static_cast<const jj_vm::ir::ConstI64 *>(lhs_operation);
    ASSERT_EQ(some_constant->val(), 0);
    //
    ASSERT_EQ(last_operation.rhs(), another_val);
}

}  // namespace jj_vm::testing
