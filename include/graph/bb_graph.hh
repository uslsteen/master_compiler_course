#pragma once

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <type_traits>

#include "dfs.hh"

namespace jj_ir::graph {

/**
 * @brief Base graph of basic blocks
 *        It provides root basic block (aka HEAD) and size of the graph
 *
 */
class BBGraph final {
public:
    using value_type = BasicBlock;
    using node_pointer = value_type*;
    using const_node_pointer = const value_type*;
    using node_reference = value_type&;
    using const_node_reference = const value_type&;
    using size_type = std::size_t;
    //
    //! NOTE: basically it should be iterator over succs and preds
    using node_iterator = std::vector<BasicBlock*>::const_iterator;

    // std::vector<BasicBlock*>::iterator;
    // using const_node_iterator = // std::vector<BasicBlock*>::const_iterator;

private:
    const_node_pointer m_head{};
    size_type m_size{};

public:
    BBGraph(const_node_pointer head, size_t size) noexcept
        : m_head(head), m_size(size) {}

    const_node_pointer head() const noexcept { return m_head; }
    size_type size() const noexcept { return m_size; }

    node_iterator succs_begin(const_node_pointer pnode) const noexcept {
        return pnode->succs().begin();
    }

    node_iterator preds_begin(const_node_pointer pnode) const noexcept {
        return pnode->preds().begin();
    }

    node_iterator succs_end(const_node_pointer pnode) const noexcept {
        return pnode->succs().end();
    }

    node_iterator preds_end(const_node_pointer pnode) const noexcept {
        return pnode->preds().end();
    }

    /**
     * @brief Function to dump Basic Block graph into graphiz
     *        Inspired by Derzhavin A.
     *
     * @param[in] os
     * @param[in] graph_name
     */
    void dot_dump(std::ostream& os,
                  const std::string& graph_name = "bb_graph") const {
        //
        os << "digraph " << graph_name << "{\n";
        //
        auto dump_visitor = [&](const_node_pointer pnode) {
            auto get_name = [](const_node_pointer other) {
                std::ostringstream ss;
                ss << "bb" << other->bb_id();
                return ss.str();
            };

            auto&& name = get_name(pnode);
            os << name << " [label=" << '"' << pnode->bb_id() << "\"];\n";

            for (auto&& pred : pnode->preds())
                os << get_name(pred) << " -> " << name << ";\n";
        };
        //
        algo_impl::deep_first_search_preoder(*this, dump_visitor);
        os << "}";
    }

    void dot_dump(const std::filesystem::path& path,
                  const std::string& name = "bb_graph") const {
        std::ofstream output{path};
        assert(output.is_open() && "Error : output path cannot be open");
        dot_dump(output, name);
    }
};

}  // namespace jj_ir::graph