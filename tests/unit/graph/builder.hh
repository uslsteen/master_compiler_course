#pragma once

#include <gtest/gtest.h>

#include <IR/basic_block.hh>
#include <IR/function.hh>
#include <initializer_list>
#include <memory>
#include <vector>

#include "graph/dom3.hh"

namespace jj_ir::testing {
//
class TestBuilder : public ::testing::Test {
protected:
    std::unique_ptr<jj_ir::Function> m_func{};
    std::vector<jj_ir::BasicBlock*> m_basic_blocks{};
    //
    TestBuilder() = default;

    void init_test(uint32_t graph_size, const std::string& func_name = {}) {
        assert(graph_size >= 0 &&
               "Error : graph test size should be non-negative");
        //
        m_basic_blocks.clear();
        m_basic_blocks.resize(graph_size);

        m_func = std::make_unique<Function>();
        //
        std::generate(m_basic_blocks.begin(), m_basic_blocks.end(),
                      [this] { return m_func->create<BasicBlock>(); });
    }

    size_t letter_cast(char letter) {
        return static_cast<size_t>(letter) - 'A';
    }

    void create_edge(size_t succ_id, size_t pred_id) {
        jj_ir::BasicBlock::link_blocks(m_basic_blocks.at(succ_id),
                                       m_basic_blocks.at(pred_id));
    }

    bool check_succ(size_t succ_id, size_t pred_id) {
        return (m_basic_blocks.at(pred_id)->get_next() ==
                m_basic_blocks.at(succ_id));
    }

    void create_edge(char succ_let, char pred_let) {
        create_edge(letter_cast(succ_let), letter_cast(pred_let));
    }
};

class DFSInterface : public TestBuilder {
protected:
    std::vector<uint32_t> preorder_ref{};
    std::vector<uint32_t> postorder_ref{};

    DFSInterface(std::initializer_list<uint32_t> preord,
                 std::initializer_list<uint32_t> postord)
        : preorder_ref(preord), postorder_ref(postord) {}

    bool check_order(const std::vector<const jj_ir::BasicBlock*>& bbs,
                     const std::vector<uint32_t>& ref_order) {
        if (bbs.size() != ref_order.size())
            return false;
        else {
            for (size_t i = 0; i < bbs.size(); ++i)
                if (bbs.at(i)->bb_id() != ref_order.at(i)) return false;
            return true;
        }
    }
};

}  // namespace jj_ir::testing
