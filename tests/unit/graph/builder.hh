#pragma once

#include <gtest/gtest.h>

#include <IR/basic_block.hh>
#include <IR/function.hh>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <vector>

#include "analysis/linear_order.hh"
#include "analysis/liveness_analyzer.hh"
#include "analysis/loop_analyzer.hh"
#include "analysis/reg_alloc.hh"
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
        std::generate(m_basic_blocks.begin(), m_basic_blocks.end(), [this] {
            return m_func->create<jj_vm::ir::BasicBlock>();
        });
    }

    size_t letter_cast(char letter) {
        return static_cast<size_t>(letter) - 'A';
    }

    bool check_succ(size_t succ_id, size_t pred_id) {
        return (m_basic_blocks.at(pred_id)->get_next() ==
                m_basic_blocks.at(succ_id));
    }

    void make_edge(size_t succ_id, size_t pred_id) {
        jj_vm::ir::BasicBlock::link_blocks(m_basic_blocks.at(succ_id),
                                           m_basic_blocks.at(pred_id));
    }

    void create_edge(char succ_let, char pred_let) {
        make_edge(letter_cast(succ_let), letter_cast(pred_let));
    }

    jj_vm::ir::BasicBlock* get_bb(size_t id) const {
        return m_basic_blocks.at(id);
    }
};

class DFSInterface : public TestBuilder {
protected:
    std::vector<uint32_t> preorder_ref{};
    std::vector<uint32_t> postorder_ref{};

    DFSInterface(std::initializer_list<uint32_t> preord,
                 std::initializer_list<uint32_t> postord)
        : preorder_ref(preord), postorder_ref(postord) {}

    bool check_order(const std::vector<jj_vm::ir::BasicBlock*>& bbs,
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

/**
 * @brief
 *
 */
class LoopInterface : public TestBuilder {
protected:
    using loop_tree = jj_vm::analysis::loop::LoopTree<jj_vm::graph::BBGraph>;
    using loop_base = loop_tree::loop_base;
    using loop_base_pointer = loop_tree::loop_base_pointer;
    //
    using node_pointer = loop_tree::node_pointer;

    LoopInterface() = default;

    jj_vm::analysis::loop::LoopTree<jj_vm::graph::BBGraph> m_tree;
    //
    void build() {
        m_tree = jj_vm::analysis::loop::LoopTreeBuilder<
            jj_vm::graph::BBGraph>::build(m_func->bb_graph());
    }

    //
    bool check_header(const loop_base* loop,
                      std::optional<std::size_t> opt_id = std::nullopt) const {
        auto expected_bb = opt_id ? m_basic_blocks.at(*opt_id) : nullptr;
        return loop->header() == expected_bb;
    }

    //
    auto* get_loop(size_t id) const {
        auto res = m_tree.find(m_basic_blocks.at(id));
        assert(res != m_tree.end() &&
               "Error : loop three has no such basic blocks");
        return res->second;
    }

    //
    bool check_back_edges(const loop_base* loop,
                          std::initializer_list<size_t> expected) {
        std::set<node_pointer> expected_back_edges{};
        for (auto id : expected)
            expected_back_edges.insert(m_basic_blocks.at(id));
        return loop->back_edges() == expected_back_edges;
    }

    //
    bool check_inners(const loop_base* loop,
                      const loop_base* other = nullptr) const {
        if (other == nullptr) return loop->inners().empty();
        //
        return loop->inners().find(other) != loop->inners().end();
    }
};

class LinearOrderInterface : public TestBuilder {
protected:
    using OrderTy = typename jj_ir::analysis::order::LinearOrderBuilder<
        jj_vm::graph::BBGraph>::OrderTy;

    OrderTy m_linear_order;

    LinearOrderInterface() = default;

    void build() {
        jj_vm::graph::BBGraph graph = m_func->bb_graph();
#if 0
        std::filesystem::path path{"order_test.dot"};
        graph.dot_dump(path, "order_test");
#endif
        auto loop_tree = jj_vm::analysis::loop::LoopTreeBuilder<
            jj_vm::graph::BBGraph>::build(graph);

        m_linear_order = jj_ir::analysis::order::LinearOrderBuilder<
            jj_vm::graph::BBGraph>::build(m_func->bb_graph(), loop_tree);
    }

    bool check_order(const std::vector<uint32_t>& ref_order) {
        if (m_linear_order.size() != ref_order.size())
            return false;
        else {
            for (size_t i = 0; i < ref_order.size(); ++i)
                if (m_linear_order.at(i)->bb_id() != ref_order.at(i))
                    return false;
            return true;
        }
    }
};

class LivenessInterface : public TestBuilder {
protected:
    using LiveIntevalTy = jj_vm::ir::LiveInterval;

    jj_vm::analysis::liveness::LivenessAnalyzer<jj_vm::graph::BBGraph>
        m_analyzer;

    std::vector<std::size_t> ref_live_nums;
    std::vector<LiveIntevalTy> ref_life_ranges;

    LivenessInterface(std::initializer_list<std::size_t> live_nums,
                      std::vector<LiveIntevalTy>&& life_ranges)
        : ref_live_nums(live_nums), ref_life_ranges(life_ranges){};

    void build() {
        m_analyzer = jj_vm::analysis::liveness::LivenessBuilder<
            jj_vm::graph::BBGraph>::build(m_func->bb_graph());
    }
};

class RegAllocInterface : public TestBuilder {
protected:
    static constexpr std::size_t kRegsNum = 3;
    using RegAllocTy =
        jj_vm::analysis::regalloc::RegAllocAnalyzer<jj_vm::graph::BBGraph,
                                                    kRegsNum>;
    using LocationTy = jj_vm::analysis::regalloc::Location;

    std::unique_ptr<RegAllocTy> m_regalloc;
    std::vector<LocationTy> ref_locations;

    RegAllocInterface(std::vector<LocationTy>&& locations)
        : ref_locations(locations) {}

    void build() {
        m_regalloc = std::make_unique<std::decay_t<decltype(*m_regalloc)>>(
            m_func->bb_graph());
        //
        m_regalloc->linear_scan();
    }
};

}  // namespace jj_vm::testing
