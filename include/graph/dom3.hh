#pragma once

#include <stack>

#include "IR/basic_block.hh"
#include "dfs.hh"
#include "dom3_base.hh"
#include "dsu.hh"

namespace jj_vm::graph::dom3_impl {
//

using BBDom3Node = dom3_impl::Dom3NodeBase<jj_vm::ir::BasicBlock>;

template <class GraphTy>
class DomTree final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    // using const_node_pointer = typename GraphTy::const_node_pointer;
    using node_iterator = typename GraphTy::node_iterator;
    using id_type = typename GraphTy::value_type::id_type;
    //
private:
    std::unordered_map<id_type, Dom3NodeBase<value_type>> m_data{};

    template <typename T>
    friend class DomTreeBuilder;
    //
public:
    DomTree() = default;

    bool dominates(node_pointer dominator,
                   node_pointer dominatee) const {
        if (dominator == dominatee) return true;

        std::stack<id_type> stack{};
        stack.push(dominator->bb_id());
        //
        while (!stack.empty()) {
            auto cur_id = stack.top();
            stack.pop();

            auto res = m_data.find(cur_id);

            if (res == m_data.end()) continue;

            for (auto &&imm_dominated : res->second.idommed()) {
                if (dominatee == imm_dominated) return true;
                //
                stack.push(imm_dominated->bb_id());
            }
        }
        return false;
    }

    auto size() const noexcept { return m_data.size(); }

    auto insert(const std::pair<id_type, Dom3NodeBase<value_type>> &val) {
        return m_data.insert(val);
    }

    auto begin() const noexcept { return m_data.begin(); }
    auto end() const noexcept { return m_data.end(); }

private:
    node_iterator succs_begin(node_pointer pnode) const noexcept {
        return pnode->succs().begin();
    }

    node_iterator preds_begin(node_pointer pnode) const noexcept {
        return pnode->preds().begin();
    }

    node_iterator succs_end(node_pointer pnode) const noexcept {
        return pnode->succs().end();
    }

    node_iterator preds_end(node_pointer pnode) const noexcept {
        return pnode->preds().end();
    }
};

/**
 * @brief Builder that provied creating & initilization Dominator Tree
 *
 *        Data structure inialization complexity O((N + M)logN)
 *
 * @tparam GraphTy
 */
template <class GraphTy>
class DomTreeBuilder final {
public:
    using value_type = typename GraphTy::value_type;
    using id_type = typename value_type::id_type;
    using node_pointer = typename GraphTy::node_pointer;
    //using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;
    //
    using DSUTy = typename jj_vm::graph::dsu_impl::DSU<GraphTy>;

private:
    //! NOTE: Next comments were taken from the article
    //!       Dominator Tree of a Directed Graph
    //!              by Tanuj Khattar
    //!
    //! Therefore I decided to pass it here to save connection with original
    //! article

    //! NOTE: mapping of i’th node to its new index, equal to the arrival
    //! time (id number after DFS) of node in
    std::vector<node_pointer> m_dfs_nodes{};

    //! NOTE: mapping of node id to DFS time (cost of arrival that based on
    //! order)
    std::unordered_map<id_type, id_type> m_dfs_labels{};

    //! NOTE: parent of node i in dfs tree
    std::unordered_map<id_type, node_pointer> m_dfs_parents{};

    //! NOTE: label of immediate-dominator of the i’th node
    std::vector<id_type> m_idoms{};

    //! NOTE: label of semi-dominator of the i’th node
    std::vector<id_type> m_sdoms{};

    //! NOTE: For a vertex i, it stores a list of vertices for which i is the
    //! semi-dominator
    std::vector<std::vector<node_pointer>> m_sdommed_bucket{};

    DomTree<GraphTy> m_tree;

    explicit DomTreeBuilder(const GraphTy &graph)
        : m_idoms(graph.size()),
          m_sdoms(graph.size()),
          m_sdommed_bucket(graph.size()) {
        //
        m_dfs_nodes.reserve(graph.size());
        //
        make_dfs(graph);
        build_sdoms();
        build_idoms();
    }

    void reset() {
        m_dfs_nodes.clear();
        m_dfs_labels.clear();
        m_dfs_parents.clear();
        m_idoms.clear();
        m_sdoms.clear();
        m_sdommed_bucket.clear();
    }

    void make_dfs(const GraphTy &graph) {
        //
        auto dom3_visitor =
            [this, prev_node = graph.head()](node_pointer node) mutable {
                const auto dfs_cost = m_dfs_nodes.size();
                const auto id = node->bb_id();
                //
                m_dfs_nodes.push_back(node);
                //
                m_dfs_labels[id] = dfs_cost;
                m_sdoms[dfs_cost] = dfs_cost;
                m_idoms[dfs_cost] = dfs_cost;

                m_dfs_parents[id] = prev_node;
                prev_node = node;
            };
        //
        jj_vm::graph::dfs_impl::deep_first_search_preoder(graph, dom3_visitor);
    }

    /**
     * @brief
     *
     * @param[in] node
     * @return auto
     */
    auto min_sdom(node_pointer node, DSUTy &dsu) {
        const auto node_dfs_cost = m_dfs_labels[node->bb_id()];
        auto &sdom = m_sdoms[node_dfs_cost];
        //
        auto cur_it = m_tree.preds_begin(node), end = m_tree.preds_end(node);

        for (; cur_it != end; ++cur_it) {
            auto found_neighb = m_dfs_labels[dsu.find(*cur_it)->bb_id()];
            sdom = std::min(sdom, m_sdoms[found_neighb]);
        }

        return sdom;
    }

    /**
     * @brief
     *
     */
    void build_sdoms() {
        jj_vm::graph::dsu_impl::DSU<GraphTy> dsu{m_sdoms, m_dfs_labels,
                                                 m_dfs_nodes};
        const auto rend = m_dfs_nodes.rend();
        //
        for (auto cur_it = m_dfs_nodes.rbegin(); cur_it != rend; ++cur_it) {
            auto node = *cur_it;
            auto sdom = min_sdom(node, dsu);

            bool isnt_first = (node != m_dfs_nodes.front());
            //
            if (isnt_first)
                m_sdommed_bucket[m_dfs_nodes[sdom]->bb_id()].push_back(node);

            //! NOTE: Initialization
            for (const auto &dominatee : m_sdommed_bucket[node->bb_id()]) {
                const auto min_semi_dom = dsu.find(dominatee);

                auto sdomin_id = m_dfs_labels[dominatee->bb_id()];
                auto min_sdomin_id = m_dfs_labels[min_semi_dom->bb_id()];
                auto dominatee_sdom_id = m_sdoms[sdomin_id];

                if (dominatee_sdom_id == m_sdoms[min_sdomin_id])
                    m_idoms[sdomin_id] = dominatee_sdom_id;
                else
                    m_idoms[sdomin_id] = min_sdomin_id;
            }

            if (isnt_first) dsu.merge(node, m_dfs_parents[node->bb_id()]);
        }
    }

    /**
     * @brief
     *
     */
    void build_idoms() {
        const auto end = m_dfs_nodes.end();
        //
        for (auto cur_it = std::next(m_dfs_nodes.begin()); cur_it != end;
             ++cur_it) {
            const auto node = *cur_it;

            const auto node_dist_cost =
                std::distance(m_dfs_nodes.begin(), cur_it);
            //
            auto &idom_dist_cost = m_idoms[node_dist_cost];

            if (idom_dist_cost != m_sdoms[node_dist_cost])
                idom_dist_cost = m_idoms[idom_dist_cost];

            auto idom_node = m_dfs_nodes[idom_dist_cost];

            const auto res = m_tree.insert(
                std::make_pair(idom_node->bb_id(), Dom3NodeBase{idom_node}));

            res.first->second.push_idom(node);
        }
    }

    auto tree() const noexcept { return m_tree; }

public:
    static auto build(const GraphTy &graph) {
        DomTreeBuilder<GraphTy> builder{graph};
        return builder.tree();
    }
};

}  // namespace jj_vm::graph::dom3_impl
