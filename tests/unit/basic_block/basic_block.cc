#include "gtest/gtest.h"
#include "IR/basic_block.hh"

TEST(BasicBlock, init) {
    jj_ir::BasicBlock bb{1};
}