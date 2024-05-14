#include <cstdint>

#include "../graph/builder.hh"
//
#include "opt_passes/constant_fold.hh"

namespace jj_vm::testing {
class FoldingBuilder : public TestBuilder {
protected:
    jj_vm::passes::ConstantFold m_pass;
    jj_vm::ir::IRBuilder m_builder{};
};

TEST_F(FoldingBuilder, MUL) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);
    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(2);

    auto *result = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::MUL,
                                                         lval, rval);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 3);

    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::CONST);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);

    const auto &folded_const =
        static_cast<const jj_vm::ir::ConstI64&>(last_instr);

    ASSERT_EQ(folded_const.val(), 64);
}

TEST_F(FoldingBuilder, SHR) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(1);
    auto *result = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::SHR,
                                                         lval, rval);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 3);

    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::CONST);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);

    const auto&folded_const =
        static_cast<const jj_vm::ir::ConstI64 &>(last_instr);

    ASSERT_EQ(folded_const.val(), 16);
}

TEST_F(FoldingBuilder, XOR) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);

    auto *lval = m_builder.create<jj_vm::ir::ConstI64>(32);
    auto *rval = m_builder.create<jj_vm::ir::ConstI64>(1);
    auto *result = m_builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::XOR,
                                                         lval, rval);
    //
    m_pass.run(m_func.get());

    ASSERT_EQ(bb0->size(), 3);

    const auto &last_instr = bb0->back();
    //
    ASSERT_EQ(last_instr.opcode(), jj_vm::ir::Opcode::CONST);
    ASSERT_EQ(last_instr.type(), jj_vm::ir::TypeId::I64);

    const auto &folded_const =
        static_cast<const jj_vm::ir::ConstI64&>(last_instr);

    ASSERT_EQ(folded_const.val(), 33);
}
}
