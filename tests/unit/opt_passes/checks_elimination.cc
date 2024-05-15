#include <cstdint>

#include "../graph/builder.hh"
//
#include "opt_passes/checks_elimination.hh"

using namespace jj_vm::ir;

namespace jj_vm::testing {
class EliminationBuilder : public TestBuilder {
protected:
    jj_vm::passes::ChecksElimination m_pass;
    IRBuilder m_builder{};
};

/*
 bb0:
     v0: i64 = ConstInstr param1;
     v1: i64 = ConstInstr param2;
     v2: i64 = CONST 20;
     branch bb1;

 bb1:
     NullCheck(v0)
     v3: i64 = DIV v2, v0;   <--------- Elimination
     NullCheck(v0);
     v4: i64 = DIV v1, v0;
     v5: i1 = EQ v3, v4;
     if (v5, bb2, bb3);

 bb2:
     NullCheck(v0)           <--------- Elimination
     v6: i64 = SUB v0, v2;
     NullCheck(v6)
     RET v6;

 bb3:
     RET v1;

 */
TEST_F(EliminationBuilder, NullCheck) {
    init_test(4);

    auto bb0 = get_bb(0);
    auto bb1 = get_bb(1);
    auto bb2 = get_bb(2);
    auto bb3 = get_bb(3);

    //
    m_builder.set_insert_point(bb0);

    auto *v0 = m_builder.create<ParamInstr>("param1", TypeId::I64);
    auto *v1 = m_builder.create<ParamInstr>("param2", TypeId::I64);
    auto *v2 = m_builder.create<ConstI64>(20);
    //
    auto bb0_to_bb1 = m_builder.create<BranchInstr>(bb1);

    m_builder.set_insert_point(bb1);

    auto *null_check_0 = m_builder.create<UnaryInstr>(Opcode::NULL_CHECK, v0);
    auto *v3 = m_builder.create<BinInstr>(Opcode::DIV, v2, v0);
    auto *null_check_1 = m_builder.create<UnaryInstr>(Opcode::NULL_CHECK, v0);
    auto *v4 = m_builder.create<BinInstr>(Opcode::DIV, v1, v0);
    auto *v5 = m_builder.create<BinInstr>(Opcode::EQ, v3, v4);
    auto *bb1_if = m_builder.create<IfInstr>(bb2, bb3, v5);

    m_builder.set_insert_point(bb2);

    auto *null_check_2 = m_builder.create<UnaryInstr>(Opcode::NULL_CHECK, v0);
    auto v6 = m_builder.create<BinInstr>(Opcode::SUB, v0, v2);
    auto *null_check_3 = m_builder.create<UnaryInstr>(Opcode::NULL_CHECK, v6);
    //
    m_builder.create<RetInstr>(v6);

    m_builder.set_insert_point(bb3);

    m_builder.create<RetInstr>(v1);
    m_pass.run(m_func.get());

    //! NOTE: CHECK ELIMINATION
    {
        ASSERT_EQ(v3->get_prev(), null_check_0);
        ASSERT_EQ(v3->get_next(), v4);
        ASSERT_EQ(&bb2->front(), v6);
        ASSERT_EQ(v6->get_next(), null_check_3);
    }
}

/*
 bb0:
     v0: i64 = ConstInstr param1;
     v1: i64 = ConstInstr param2;
     v2: i64 = CONST 20;
     branch bb1;

 bb1:
     BOUNDS_CHECK(v0, v2)
     v3: i64 = DIV v1, v2;
     v4: i1 = EQ v1, v4;
     if (v4, bb2, bb3);

 bb2:
     BOUNDS_CHECK(v0, v2)   <--------- Elimination
     v5: i64 = ADD v3, v2;
     RET v5;

 bb3:
     v6: i64 = CONST 30;
     BOUNDS_CHECK(v0, v6)
     v7: i64 = ADD v3, v2;
     RET v7;

 */
TEST_F(EliminationBuilder, BoundsCheck) {
    init_test(4);

    auto bb0 = get_bb(0);
    auto bb1 = get_bb(1);
    auto bb2 = get_bb(2);
    auto bb3 = get_bb(3);

    //
    m_builder.set_insert_point(bb0);

    auto *v0 = m_builder.create<ParamInstr>("param1", TypeId::I64);
    auto *v1 = m_builder.create<ParamInstr>("param2", TypeId::I64);
    auto *v2 = m_builder.create<ConstI64>(20);
    //
    auto bb0_to_bb1 = m_builder.create<BranchInstr>(bb1);

    m_builder.set_insert_point(bb1);

    auto *bounds_check_1 =
        m_builder.create<BinInstr>(Opcode::BOUNDS_CHECK, v0, v2);
    auto *v3 = m_builder.create<BinInstr>(Opcode::DIV, v1, v2);
    auto *v4 = m_builder.create<BinInstr>(Opcode::EQ, v1, v3);
    auto *bb1_if = m_builder.create<IfInstr>(bb2, bb3, v4);

    m_builder.set_insert_point(bb2);

    auto *bounds_check_2 =
        m_builder.create<BinInstr>(Opcode::BOUNDS_CHECK, v0, v2);
    auto *v5 = m_builder.create<BinInstr>(Opcode::ADD, v3, v2);
    m_builder.create<RetInstr>(v5);

    m_builder.set_insert_point(bb3);

    auto *v6 = m_builder.create<ConstI64>(30);
    auto *bounds_check_3 =
        m_builder.create<BinInstr>(Opcode::BOUNDS_CHECK, v0, v6);
    auto v7 = m_builder.create<BinInstr>(Opcode::ADD, v3, v6);
    m_builder.create<RetInstr>(v7);

    m_pass.run(m_func.get());

    //! NOTE: CHECK ELIMINATION
    {
        ASSERT_EQ(&bb1->front(), bounds_check_1);
        ASSERT_EQ(&bb2->front(), v5);
        ASSERT_EQ(v6->get_next(), bounds_check_3);
    }
}
}  // namespace jj_vm::testing
