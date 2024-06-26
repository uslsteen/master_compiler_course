#pragma once

#include "graph/dfs.hh"
#include "graph/dom3.hh"
#include "loop_base.hh"
//
#include <list>
#include <unordered_map>
#include <vector>

namespace jj_vm::analysis::loop {

template <typename GraphTy>
class LoopTreeBuilder;

/**
 * @brief
 *
 * @tparam GraphTy
 */
template <class GraphTy>
class LoopTree final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;
    //
    using loop_base = LoopNodeBase<GraphTy>;
    using loop_base_pointer = loop_base*;

private:
    friend LoopTreeBuilder<GraphTy>;

    std::unordered_map<node_pointer, loop_base_pointer> m_data{};
    std::list<loop_base> m_loops;

public:
    LoopTree() = default;

    LoopTree(const LoopTree&) = default;
    LoopTree(LoopTree&&) = default;

    LoopTree& operator=(const LoopTree&) = default;
    LoopTree& operator=(LoopTree&&) = default;

    //
    // auto insert(node_pointer node, loop_base_pointer loop) {
    //     return m_data.insert(std::make_pair(node, loop));
    // }
    //
    // //
    // auto insert(loop_base&& val) { return m_loops.insert(m_loops.end(), val);
    // }
    //
    // //
    // auto check_loop(node_pointer pnode) {
    //     return m_data.find(pnode) != m_data.end();
    // }
    //
    auto get_loop(node_pointer pnode) const { return m_data.at(pnode); }

    //
    template <class... Args>
    auto emplace_back(Args&&... args) & {
        return &(m_loops.emplace_back(std::forward<Args>(args)...));
    }

    //
    auto find(node_pointer node) const noexcept {
        auto res = m_data.find(node);
        return res;
    }

    auto size() const noexcept { return m_data.size(); }

    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    // node_iterator succs_begin(node_pointer pnode) const noexcept {
    //     return pnode->succs().begin();
    // }
    //
    // node_iterator preds_begin(node_pointer pnode) const noexcept {
    //     return pnode->preds().begin();
    // }
    //
    // node_iterator succs_end(node_pointer pnode) const noexcept {
    //     return pnode->succs().end();
    // }
    //
    // node_iterator preds_end(node_pointer pnode) const noexcept {
    //     return pnode->preds().end();
    // }
};

/**
 * @brief
 *
 * @tparam GraphTy
 */
template <class GraphTy>
class LoopTreeBuilder final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    // using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    using loop_base = typename LoopTree<GraphTy>::loop_base;
    using loop_base_pointer = typename LoopTree<GraphTy>::loop_base_pointer;
    //
private:
    using dom_tree = typename jj_vm::graph::dom3_impl::DomTree<GraphTy>;
    //
    dom_tree m_dom3{};
    std::vector<node_pointer> m_dfs_nodes{};
    std::unordered_map<node_pointer, graph::dfs_impl::DFSColors> m_marked{};
    //
    //! NOTE: internals of loop tree
    std::unordered_map<node_pointer, loop_base_pointer> m_data{};
    std::list<loop_base> m_loops;
    //
public:
    LoopTreeBuilder() = default;

    explicit LoopTreeBuilder(const GraphTy& graph)
        : m_dom3{
              jj_vm::graph::dom3_impl::DomTreeBuilder<GraphTy>::build(graph)} {
        //
        std::vector<node_pointer> free_loop_nodes{};
        //
        collect_back_edges(graph, free_loop_nodes);
        populate_loops(graph);
        init_root_loop(free_loop_nodes);
    }

    LoopTreeBuilder(const LoopTreeBuilder<GraphTy>&) = delete;
    LoopTreeBuilder(LoopTreeBuilder<GraphTy>&&) = delete;

    LoopTreeBuilder<GraphTy>& operator=(const LoopTreeBuilder<GraphTy>&) =
        delete;
    LoopTreeBuilder<GraphTy>& operator=(LoopTreeBuilder<GraphTy>&&) = delete;

    /**
     * @brief Custom Loop analyzer visitor to provide Post Order travel
     *                         over the callgraph
     */
    class LoopAnalyzerVisitor
        : public jj_vm::graph::dfs_impl::IVisitor<GraphTy> {
        //
        using value_type = typename GraphTy::value_type;
        using node_pointer = typename GraphTy::node_pointer;
        // using const_node_pointer = typename GraphTy::const_node_pointer;
        //
        const dom_tree& m_dom3;
        std::vector<node_pointer>& m_dfs_nodes;
        std::vector<node_pointer>& m_free_loop_nodes;
        std::unordered_map<node_pointer, loop_base_pointer>& m_data{};
        std::list<loop_base>& m_loops;

    public:
        LoopAnalyzerVisitor(
            const dom_tree& dom3, std::vector<node_pointer>& dfs_nodes,
            std::vector<node_pointer>& free_loop_nodes,
            std::unordered_map<node_pointer, loop_base_pointer>& data,
            std::list<loop_base>& loops)
            : m_dom3(dom3),
              m_dfs_nodes(dfs_nodes),
              m_free_loop_nodes(free_loop_nodes),
              m_data(data),
              m_loops(loops) {}

        //
        //! NOTE: if node w/ a grey mark, then edge to this block is back edge
        void back_edge(node_pointer src, node_pointer dst) override {
            // auto res = m_data[dst] = nullptr;
            auto res = m_data.insert(std::make_pair(dst, nullptr));

            //! NOTE: right now it can be nullptr
            auto& cur_loop = res.first->second;

            //! NOTE: check if m_tree hasn't current loop
            if (res.second) {
                //! NOTE: check that dst doesn't dominate back edge
                bool is_reducible = m_dom3.dominates(dst, src);

                auto insert_it =
                    m_loops.insert(m_loops.end(), loop_base{dst, is_reducible});
                // m_tree.emplace_back(dst, is_reducible);
                cur_loop = &(*insert_it);
            }

            //! NOTE: anyway tie current loop with src node
            m_data[src] = cur_loop;
            cur_loop->add_back_edge(src);
        }

        //
        //! NOTE: Post Order Travel
        void finish_node(node_pointer pnode) override {
            if (m_data.find(pnode) != m_data.end())
                m_dfs_nodes.push_back(pnode);
            else
                m_free_loop_nodes.push_back(pnode);
        }
    };

    void collect_back_edges(const GraphTy& graph,
                            std::vector<node_pointer>& free_loop_nodes) {
        //
        LoopAnalyzerVisitor vis{m_dom3, m_dfs_nodes, free_loop_nodes, m_data,
                                m_loops};
        jj_vm::graph::dfs_impl::deep_first_search(graph, vis);
        std::reverse(m_dfs_nodes.begin(), m_dfs_nodes.end());
    }

    //
    void populate_loops(const GraphTy& graph) {
        for (auto&& node : m_dfs_nodes) {
            auto res = m_data.find(node);
            //
            assert(res != m_data.end() &&
                   "Error : loop doesn't contains current node");

            auto* cur_loop = res->second;

            if (cur_loop->is_reducible()) {
                //! NOTE: for all back edges run loop search
                auto&& cur_header = cur_loop->header();

                //! NOTE: header block is marked firstly to stop search on it
                m_marked[cur_header] = graph::dfs_impl::DFSColors::BLACK;

                //! NOTE: dfs from src of back edges to find block in the loop
                for (auto* be_src : cur_loop->back_edges())
                    loop_search(be_src, cur_loop);

                //! NOTE: unmark header
                m_marked[cur_header] = graph::dfs_impl::DFSColors::WHITE;

            } else {
                //! NOTE: append all src of back edges in the loop
                for (auto* cur_be : cur_loop->back_edges()) {
                    auto be_src_loop = m_data.find(cur_be);

                    if (be_src_loop != m_data.end()) m_data[cur_be] = cur_loop;
                }
            }
        }
    }

    //
    void loop_search(node_pointer cur_node, loop_base_pointer cur_loop) {
        auto cur_found = m_marked.find(cur_node);
        if (cur_found != m_marked.end() &&
            cur_found->second == graph::dfs_impl::DFSColors::BLACK)
            return;

        m_marked[cur_node] = graph::dfs_impl::DFSColors::BLACK;
        //
        auto be_loop_it = m_data.find(cur_node);

        //! NOTE: if block without loop => add to the current loop
        if (be_loop_it == m_data.end()) {
            cur_loop->add_node(cur_node);
            m_data[cur_node] = cur_loop;
        }
        //! NOTE: if block in other loop (inner) link outer and inner loop
        else {
            auto inner_loop = be_loop_it->second;

            if (inner_loop != cur_loop && inner_loop->outer() == nullptr)
                cur_loop->add_inner(inner_loop);
        }

        auto cur_it = cur_node->preds().begin(), end = cur_node->preds().end();

        //! NOTE: search runs for all it preds
        for (; cur_it != end; ++cur_it) loop_search(*cur_it, cur_loop);
    }

    //
    void init_root_loop(const std::vector<node_pointer>& free_loop_nodes) {
        auto root_loop =
            &(*m_loops.insert(m_loops.end(), loop_base{nullptr, false, true}));

        //
        for (auto&& free_node : free_loop_nodes) {
            auto res = m_data.insert(std::pair{free_node, root_loop});
            if (res.second) root_loop->add_node(free_node);
        }

        //! NOTE: tie all inner loop
        for (auto&& cur_loop : m_loops) {
            if (root_loop != &cur_loop && cur_loop.outer() == nullptr)
                root_loop->add_inner(&cur_loop);
        }
    }

public:
    static LoopTree<GraphTy> build(const GraphTy& graph) {
        LoopTreeBuilder<GraphTy> builder{graph};
        LoopTree<GraphTy> tree{};
        //
        tree.m_loops = std::move(builder.m_loops);
        tree.m_data = std::move(builder.m_data);
        //
        return tree;
    }
};

}  // namespace jj_vm::analysis::loop
