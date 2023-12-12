#pragma once

#include <gtest/gtest.h>

#include <IR/basic_block.hh>
#include <IR/function.hh>
#include <initializer_list>
#include <memory>
#include <vector>

#include "graph/dom3.hh"

namespace jj_vm::testing {
//
class TestBuilder : public ::testing::Test {
protected:
    std::unique_ptr<jj_vm::ir::Function> m_func{};
    std::vector<jj_vm::ir::BasicBlock*> m_basic_blocks{};
    //
    TestBuilder() = default;

    void init_test(uint32_t graph_size, const std::string& func_name = {}) {
        assert(graph_size >= 0 &&
               "Error : graph test size should be non-negative");
        //
        m_basic_blocks.clear();
        m_basic_blocks.resize(graph_size);

        m_func = std::make_unique<jj_vm::ir::Function>();
        //
        std::generate(m_basic_blocks.begin(), m_basic_blocks.end(),
                      [this] { return m_func->create<jj_vm::ir::BasicBlock>(); });
    }

    size_t letter_cast(char letter) {
        return static_cast<size_t>(letter) - 'A';
    }

    void create_edge(size_t succ_id, size_t pred_id) {
        jj_vm::ir::BasicBlock::link_blocks(m_basic_blocks.at(succ_id),
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

    bool check_order(const std::vector<const jj_vm::ir::BasicBlock*>& bbs,
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

class DominatorInterface : public TestBuilder {
protected:
    DominatorInterface() = default;

    void build() {
        m_tree = jj_vm::graph::dom3_impl::DomTreeBuilder<
            jj_vm::graph::BBGraph>::build(m_func->bb_graph());
    }
    bool is_dominator(uint32_t dominator, uint32_t dominatee) {
        return m_tree.dominates(m_basic_blocks[dominator],
                                m_basic_blocks[dominatee]);
    }

    jj_vm::graph::dom3_impl::DomTree<jj_vm::graph::BBGraph> m_tree{};
};

}  // namespace jj_vm::testing
