#pragma once

#include <gtest/gtest.h>

#include <IR/basic_block.hh>
#include <IR/function.hh>
#include <memory>
#include <vector>

namespace jj_ir::testing {
//
class TestBuilder : public ::testing::Test {
protected:
    std::unique_ptr<jj_ir::Function> m_func{};
    std::vector<jj_ir::BasicBlock*> m_basic_blocks{};
    //
    std::vector<uint32_t> preorder_ref{};
    std::vector<uint32_t> postorder_ref{};
    //
    TestBuilder(std::initializer_list<uint32_t> preorder,
                std::initializer_list<uint32_t> postorder)
        : preorder_ref(preorder), postorder_ref(postorder) {}

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

/**
 * @brief Wrapper of graph example 1, assignment 2
 *
 *            +---+
 *            | A |
 *            +---+
 *              |
 *              v
 *  +---+     +---+
 *  | C | <-- | B |
 *  +---+     +---+
 *    |         |
 *    |         v
 *    |       +---+     +---+
 *    |       | F | --> | G |
 *    |       +---+     +---+
 *    |         |         |
 *    |         v         |
 *    |       +---+       |
 *    |       | E |       |
 *    |       +---+       |
 *    |         |         |
 *    |         v         |
 *    |       +---+       |
 *    +-----> | D | <-----+
 *            +---+
 *
 */
class DFSTest1 : public TestBuilder {
protected:
    DFSTest1() : TestBuilder({0, 1, 2, 3, 5, 4, 6}, {3, 2, 4, 6, 5, 1, 0}) {}
    //
    void create_test() {
        init_test(7);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('F', 'B');
        create_edge('E', 'F');
        create_edge('G', 'F');
        create_edge('D', 'G');
        create_edge('D', 'E');
        create_edge('D', 'C');
    }
};

/**
 * @brief Wrapper of graph example 2, assignment 2
 *
 *           +---+
 *           | A |
 *           +---+
 *             |
 *             v
 *           +---+
 *   +-----> | B | --+
 *   |       +---+   |
 *   |         |     |
 *   |         |     |
 *   |         |   +---+
 *   |         |   | J |
 *   |         |   +---+
 *   |         |     |
 *   |         v     |
 *   |       +---+   |
 *   |   +-> | C | <-+
 *   |   |   +---+
 *   |   |     |
 *   |   |     v
 *   |   |   +---+
 *   |   +-- | D |
 *   |       +---+
 *   |         |
 *   |         v
 *   |       +---+
 *   |       | E | <-+
 *   |       +---+   |
 *   |         |     |
 *   |         v     |
 *   |       +---+   |
 *   |       | F | --+
 *   |       +---+
 *   |         |
 *   |         v
 * +---+     +---+
 * | H | <-- | G |
 * +---+     +---+
 *             |
 *             v
 *           +---+
 *           | I |
 *           +---+
 *             |
 *             v
 *           +---+
 *           | K |
 *           +---+
 *
 */
class DFSTest2 : public TestBuilder {
protected:
    DFSTest2()
        : TestBuilder({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 9},
                      {7, 10, 8, 6, 5, 4, 3, 2, 9, 1, 0}) {}

    void create_test() {
        init_test(11);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('J', 'B');
        create_edge('D', 'C');
        create_edge('C', 'D');
        create_edge('E', 'D');
        create_edge('F', 'E');
        create_edge('E', 'F');
        create_edge('G', 'F');
        create_edge('H', 'G');
        create_edge('I', 'G');
        create_edge('B', 'H'); 
        create_edge('K', 'I');
        create_edge('C', 'J');
    }
};

/**
 * @brief Wrapper of graph example 3, assignment 2
 *
 *       +-------------------------+
 *       |                         |
 *       |                 +---+   |
 *       |                 | A |   |
 *       |                 +---+   |
 *       |                   |     |
 *       |                   v     |
 *     +---+     +---+     +---+   |
 *     | F | <-- | E | <-- | B | <-+
 *     +---+     +---+     +---+
 *       |         |         |
 *       v         |         v
 *     +---+       |       +---+
 *  +- | H |       |       | C | <-+
 *  |  +---+       |       +---+   |
 *  |    |         |         |     |
 *  |    |         |         v     |
 *  |    |         |       +---+   |
 *  |    |         +-----> | D |   |
 *  |    |                 +---+   |
 *  |    |                   |     |
 *  |    |                   v     |
 *  |    |                 +---+   |
 *  |    +---------------> | G | --+
 *  |                      +---+
 *  |                        |
 *  |                        v
 *  |                      +---+
 *  |                      | I |
 *  |                      +---+
 *  |                        ^
 *  |                        |
 *  +------------------------+
 *
 */
class DFSTest3 : public TestBuilder {
protected:
    DFSTest3()
        : TestBuilder({0, 1, 2, 3, 6, 8, 4, 5, 7},
                      {8, 6, 3, 2, 7, 5, 4, 1, 0}) {}

    void create_test() {
        init_test(9);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('E', 'B');
        create_edge('D', 'C');
        create_edge('G', 'D');
        create_edge('D', 'E');
        create_edge('F', 'E');
        create_edge('B', 'F');
        create_edge('H', 'F');
        create_edge('C', 'G');
        create_edge('I', 'G');
        create_edge('G', 'H');
        create_edge('I', 'H');
    }
};
}  // namespace jj_ir::testing
