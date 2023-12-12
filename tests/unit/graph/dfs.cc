#include <sstream>
#include <string_view>
#include <vector>
//
#include <gtest/gtest.h>
//
#include "builder.hh"
#include "graph/dfs.hh"

namespace jj_vm::testing {

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
class DFSTest1 : public DFSInterface {
protected:
    DFSTest1() : DFSInterface({0, 1, 2, 3, 5, 4, 6}, {3, 2, 4, 6, 5, 1, 0}) {}
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
class DFSTest2 : public DFSInterface {
protected:
    DFSTest2()
        : DFSInterface({0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 9},
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
class DFSTest3 : public DFSInterface {
protected:
    DFSTest3()
        : DFSInterface({0, 1, 2, 3, 6, 8, 4, 5, 7},
                       {8, 6, 3, 2, 7, 5, 4, 1, 0}) {}
    //
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

TEST_F(DFSTest1, dfs) {
    //
    create_test();
    //
    auto&& graph = m_func->bb_graph();

    auto postorder_res = jj_vm::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_vm::graph::deep_first_search_preoder(graph);
    //
    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));
    //
    std::filesystem::path path{"example1.dot"};
    graph.dot_dump(path, "Example1");
}

TEST_F(DFSTest2, dfs) {
    //
    create_test();
    auto&& graph = m_func->bb_graph();
    //
    auto postorder_res = jj_vm::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_vm::graph::deep_first_search_preoder(graph);
    //
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));
    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    //
    std::filesystem::path path{"example2.dot"};
    graph.dot_dump(path, "Example2");
}

TEST_F(DFSTest3, dfs) {
    //
    create_test();
    //
    auto&& graph = m_func->bb_graph();

    auto postorder_res = jj_vm::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_vm::graph::deep_first_search_preoder(graph);

    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));

    std::filesystem::path path{"example3.dot"};
    graph.dot_dump(path, "Example3");
}

}  // namespace jj_vm::testing
