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

TEST_F(FoldingBuilder, ADD) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);
}

TEST_F(FoldingBuilder, SHR) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);
}

TEST_F(FoldingBuilder, XOR) {
    init_test(1);

    auto bb0 = get_bb(0);
    m_builder.set_insert_point(bb0);
}
}
