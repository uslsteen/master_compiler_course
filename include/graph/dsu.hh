#pragma once

#include <unordered_map>
#include <vector>

namespace jj_vm::graph::dsu_impl {

template <class GraphTy>
class DSU final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;
    using node_iterator = typename GraphTy::node_iterator;
    using id_type = typename GraphTy::value_type::id_type;

private:
    const std::vector<id_type> &m_sdoms;
    const std::unordered_map<id_type, id_type> &m_dfs_labels;

    //! NOTE: parent of i’th node in the forest maintained during step 2 of the
    //! algorithm
    std::vector<const_node_pointer> m_parents;

    //! NOTE: label[i] stores the vertex v with minimum sdom,
    //! lying on path from i to the root of the (dsu) tree in which node i lies
    std::vector<const_node_pointer> m_labels;

public:
    DSU(const std::vector<id_type> &sdoms,
        const std::unordered_map<id_type, id_type> &dfs_labels,
        const std::vector<const_node_pointer> &dfs_nodes)
        : m_sdoms(sdoms),
          m_dfs_labels(dfs_labels),
          m_parents(dfs_nodes),
          m_labels(dfs_nodes) {}

    DSU(const DSU &) = delete;
    DSU &operator=(const DSU &) = delete;

    DSU(DSU &&) = delete;
    DSU &operator=(DSU &&) = delete;

    ~DSU() = default;

    auto parent(const_node_pointer node) const { return access_parent(node); }

    void set_parent(const_node_pointer node, const_node_pointer par) {
        access_parent(node) = par;
    }

    auto label(const_node_pointer node) const { return access_label(node); }

    void set_label(const_node_pointer node, const_node_pointer label) {
        access_label(node) = label;
    }

    const_node_pointer find(const_node_pointer needle) {
        auto &parent_node = access_parent(needle);
        //
        if (needle == parent_node) return needle;

        auto res = find(parent_node);
        auto parent_label = label(parent_node);
        auto &needle_label = access_label(needle);

        if (sdom_id(parent_label) < sdom_id(needle_label))
            needle_label = parent_label;

        parent_node = res;
        //
        return needle_label;
    }

    void merge(const_node_pointer node, const_node_pointer parent) {
        set_parent(node, parent);
    }

private:
    //
    auto node_cost(const_node_pointer node) const {
        return m_dfs_labels.at(node->bb_id());
    }

    auto sdom_id(const_node_pointer node) const {
        return m_sdoms[node_cost(node)];
    }

    auto &access_parent(const_node_pointer node) const {
        return m_parents.at(node_cost(node));
    }
    auto &access_parent(const_node_pointer node) {
        return m_parents[node_cost(node)];
    }

    auto &access_label(const_node_pointer node) const {
        return m_labels[node_cost(node)];
    }
    auto &access_label(const_node_pointer node) {
        return m_labels[node_cost(node)];
    }
};

}  // namespace jj_vm::graph::dsu_impl
