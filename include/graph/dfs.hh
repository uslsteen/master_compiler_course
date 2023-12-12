#pragma once

#include <algorithm>
#include <cstdint>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jj_vm::graph::dfs_impl {
//

template <class GraphTy, class DFSVisitorTy>
class DFSImpl final {
public:
    using const_node_pointer = typename GraphTy::const_node_pointer;
    using node_iterator = typename GraphTy::node_iterator;

    enum class DFSColors : bool { GREY = 0, RED = 1 };

    DFSImpl(const GraphTy& graph, DFSVisitorTy vis)
        : m_graph(graph), m_vis{vis} {}
    //
private:
    GraphTy m_graph;
    DFSVisitorTy m_vis;
    //
    std::unordered_map<const_node_pointer, DFSColors> m_visited;
    std::stack<std::pair<node_iterator, const_node_pointer>> m_stack;
    //

    /**
     * @brief Function to put color of the node according searching state
     *
     * @param[in] pnode
     * @param[in] color
     */
    void put_color(const_node_pointer pnode, DFSColors color) {
        m_visited[pnode] = color;
    }

    void visit_node(const_node_pointer pnode, DFSColors color) {
        put_color(pnode, color);
        m_vis.discover_node(pnode);
        m_stack.push(std::make_pair(m_graph.succs_begin(pnode), pnode));
    }

public:
    void dfs_impl() {
        //
        auto head = m_graph.head();
        if (head == nullptr) return;

        visit_node(head, DFSColors::GREY);
        //
        while (!m_stack.empty()) {
            auto top_pair = m_stack.top();
            m_stack.pop();
            //
            auto parent = top_pair.second;
            auto succ_end = m_graph.succs_end(parent), cur_it = top_pair.first;

            for (; cur_it != succ_end; ++cur_it) {
                const auto found = m_visited.find(*cur_it);
                if (found == m_visited.end()) break;
                if (found->second == DFSColors::GREY)
                    m_vis.back_edge(parent, found->first);
            }
            //
            if (cur_it == succ_end) {
                put_color(parent, DFSColors::RED);
                m_vis.finish_node(parent);
                continue;
            }
            //
            const auto pnode = *cur_it;
            m_stack.push(std::make_pair(std::next(cur_it), parent));
            //
            visit_node(pnode, DFSColors::GREY);
        }
    }
};

template <class GraphTy>
class IVisitor {
public:
    using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    IVisitor() = default;
    virtual ~IVisitor() = default;
    //
    void virtual discover_node(const_node_pointer pnode) {}
    void virtual finish_node(const_node_pointer pnode) {}
    void virtual back_edge(const_node_pointer src, const_node_pointer dst) {}
};

template <class GraphTy, class Visitor>
class PreOrderVis : public IVisitor<GraphTy> {
    //
    Visitor m_vis;

public:
    using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    explicit PreOrderVis(Visitor vis) : m_vis{vis} {}
    //
    void discover_node(const_node_pointer pnode) override { m_vis(pnode); }
};

template <class GraphTy, class Visitor>
class PostOrderVis : public IVisitor<GraphTy> {
    //
    Visitor m_vis;

public:
    using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    explicit PostOrderVis(Visitor vis) : m_vis{vis} {}
    //
    void finish_node(const_node_pointer pnode) override { m_vis(pnode); }
};

template <typename GraphTy, typename UserVisitorTy>
void deep_first_search_preoder(const GraphTy& graph, UserVisitorTy vis) {
    //
    DFSImpl pass{graph, PreOrderVis<GraphTy, UserVisitorTy>{vis}};
    pass.dfs_impl();
}
//

template <typename GraphTy, typename UserVisitorTy>
void deep_first_search_postoder(const GraphTy& graph, UserVisitorTy vis) {
    //
    DFSImpl pass{graph, PostOrderVis<GraphTy, UserVisitorTy>{vis}};
    pass.dfs_impl();
}

}  // namespace jj_vm::graph::dfs_impl

namespace jj_vm::graph {
//

template <typename GraphTy>
auto deep_first_search_preoder(const GraphTy& graph) {
    std::vector<typename GraphTy::const_node_pointer> bbs;
    auto init_bbs = [&](auto node) { bbs.push_back(node); };
    dfs_impl::deep_first_search_preoder(graph, init_bbs);
    //
    return bbs;
}
//

template <typename GraphTy>
auto deep_first_search_postoder(const GraphTy& graph) {
    std::vector<typename GraphTy::const_node_pointer> bbs;
    auto init_bbs = [&](auto node) { bbs.push_back(node); };
    dfs_impl::deep_first_search_postoder(graph, init_bbs);
    //
    return bbs;
}

template <typename GraphTy>
auto deep_first_search_reverse_postoder(const GraphTy& graph) {
    auto bbs = deep_first_search_postoder(graph);
    std::reverse(bbs.begin(), bbs.end());
    //
    return bbs;
}
}  // namespace jj_vm::graph
