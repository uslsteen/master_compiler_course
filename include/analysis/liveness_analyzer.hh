#pragma once

#include "IR/basic_block.hh"
#include "IR/instruction.hh"
#include "linear_order.hh"
#include "loop_analyzer.hh"
//
#include <cassert>
//
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jj_vm::analysis::liveness {

template <typename GraphTy>
class LivenessBuilder;

template <typename GraphTy>
class LivenessAnalyzer final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using LiveIntevalTy = jj_vm::ir::LiveInterval;
    using LiveSetTy = std::unordered_set<ir::Value *>;
    using OrderTy =
        typename jj_ir::analysis::order::LinearOrderBuilder<GraphTy>::OrderTy;

private:
    friend LivenessBuilder<GraphTy>;

    std::unordered_map<jj_vm::ir::Value *, LiveIntevalTy> m_intervals;
    std::unordered_map<node_pointer, LiveSetTy> m_live_sets;
    //
public:
    LivenessAnalyzer() = default;

    /**
     * @brief Getters
     */
    auto &intervals() & { return m_intervals; }
    auto &&intervals() && { return std::move(m_intervals); }

    auto live_sets() const noexcept { return m_live_sets; }

    const LiveIntevalTy *get_interval(jj_vm::ir::Value *val) const {
        auto find_res = m_intervals.find(val);
        if (find_res == m_intervals.end()) return nullptr;

        return &(find_res->second);
    }
};

template <typename GraphTy>
class LivenessBuilder {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    //
    using OrderTy = typename LivenessAnalyzer<GraphTy>::OrderTy;
    using LiveSetTy = typename LivenessAnalyzer<GraphTy>::LiveSetTy;
    using LiveIntevalTy = typename LivenessAnalyzer<GraphTy>::LiveIntevalTy;

private:
    loop::LoopTree<GraphTy> m_loop_tree{};
    OrderTy order{};
    //
    std::unordered_map<ir::Value *, LiveIntevalTy> m_intervals;
    std::unordered_map<node_pointer, LiveSetTy> m_live_sets;
    //
    static constexpr std::size_t kLinStep = 1;
    static constexpr std::size_t kLiveStep = 2;

public:
    LivenessBuilder(const GraphTy &graph) {
        m_loop_tree =
            jj_vm::analysis::loop::LoopTreeBuilder<GraphTy>::build(graph);
        order = jj_ir::analysis::order::LinearOrderBuilder<GraphTy>::build(
            graph, m_loop_tree);
        build_intervals();
    }

private:
    /**
     * @brief Function to build live intervals:
                    1. assign linear/live numbers for each instruction
                    2. calucate life ranges (algorithm from lectures)
     */
    void build_intervals() {
        init_linear_num();
        calc_life_ranges();
    }

    /**
     * @brief Function to initialize line/live numbers
     */
    void init_linear_num() {
        std::size_t lin = 0, live = 0;
        //
        for (auto *bb : order) {
            const auto bb_live = live;
            for (auto &inst : *bb) {
                bool is_phi = inst.opcode() == jj_vm::ir::Opcode::PHI;

                if (!is_phi) live += kLiveStep;
                //
                inst.set_lin(lin);
                inst.set_live(is_phi ? bb_live : live);
                //
                lin += kLinStep;
            }
            //
            bb->set_interval(LiveIntevalTy{bb_live, live += kLiveStep});
        }
    }

    /**
     * @brief Function to update life range
     *
     * @param[in] value
     * @param[in] interval
     */
    void set_live_interval(jj_vm::ir::Value *value,
                           const LiveIntevalTy &interval) {
        auto [pair, insert_res] =
            m_intervals.insert(std::make_pair(value, interval));
        if (insert_res) return;
        //
        pair->second.update(interval);
    }

    /**
     * @brief Function to process loop for every basic block node
     *
     * @param[in] node - pointer to basic block
     * @param[in] set - set of ir Values
     */
    void process_loop(node_pointer node, LiveSetTy &set) {
        const auto *loop_base = m_loop_tree.get_loop(node);

        if (loop_base && loop_base->header() == node &&
            loop_base->is_reducible()) {
            auto &&cur_interval = m_live_sets[node];
            std::size_t loop_end = 0, loop_start = node->interval().begin();

            //! NOTE: Getting loop end
            for (auto *loop_node : *loop_base)
                loop_end = std::max(loop_end, loop_node->interval().end());

            for (auto *value : set)
                set_live_interval(value, LiveIntevalTy{loop_start, loop_end});
        }
    }

    /**
     * @brief Function to process each input of every instruction in basic block
     */
    void process_inputs(const jj_vm::ir::Instr &instr, LiveSetTy &set,
                        std::size_t bb_start) {
        for (auto *input : instr.inputs()) {
            assert(input != nullptr &&
                   "Error: input of instruction equals nullptr");
            set.insert(input);
            set_live_interval(input, LiveIntevalTy{bb_start, instr.live()});
        }
    }

    /**
     * @brief Function to process each instruction in reverse order
     *
     * @param[in] node - pointer to basic block
     * @param[in] set - set of ir Values
     */
    void process_instrs(node_pointer node, LiveSetTy &set) {
        for (auto it = node->rbegin(); it != node->rend(); ++it) {
            auto &instr = *it;

            if (instr.opcode() == jj_vm::ir::Opcode::PHI) return;

            auto live_num = instr.live();
            auto [pair, insert_res] = m_intervals.insert(std::make_pair(
                &instr, LiveIntevalTy{live_num, live_num + kLiveStep}));

            if (!insert_res) pair->second.set_begin(live_num);
            set.erase(&instr);

            //! NOTE: there are should be no PHI instr
            process_inputs(instr, set, node->interval().begin());
        }
    }

    /**
     * @brief Function to calculate initial liveset for block
     *
     * @param[in] node - pointer to basic block
     * @return LiveSetTy
     */
    LiveSetTy &init_live_set(node_pointer node) {
        auto [pair, insert_res] = m_live_sets.insert({node, {}});
        auto &live_set = pair->second;

        //! NOTE: union of livesets for successors
        for (auto *succ_it : node->succs()) {
            auto found_res = m_live_sets.find(succ_it);

            if (found_res == m_live_sets.end()) continue;

            auto &succ_live_set = found_res->second;
            live_set.insert(succ_live_set.begin(), succ_live_set.end());

            for (auto &instr : *succ_it) {
                if (instr.opcode() == jj_vm::ir::Opcode::PHI) {
                    const auto &phi_node =
                        static_cast<const ir::PhiInstr &>(instr);
                    //
                    for (const auto &[phi_inp, phi_bb] : phi_node.vars())
                        if (phi_bb == node) live_set.insert(phi_inp);
                }
            }
        }
        //
        return live_set;
    }

    /**
     * @brief Function to check quantity of inputs the instrction
     *
     * @param[in] instr
     */
    bool is_empty_life_range(const jj_vm::ir::Instr &instr) {
        std::set<jj_vm::ir::Opcode> no_inputs_opcodes{jj_vm::ir::Opcode::BRANCH,
                                                      jj_vm::ir::Opcode::RET,
                                                      jj_vm::ir::Opcode::IF};
        //
        return no_inputs_opcodes.find(instr.opcode()) !=
               no_inputs_opcodes.end();
    }

    /**
     * @brief Function to calculate life ranges for all inputs for each
     * instructions
     */
    void calc_life_ranges() {
        std::vector<jj_vm::ir::Instr *> no_inputs_instrs{};
        //
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            //
            auto &block = **it;
            node_pointer pnode = *it;
            auto &initial_live_set = init_live_set(pnode);

            for (auto &val : initial_live_set)
                set_live_interval(val, block.interval());

            //! NOTE: Process each instructions
            process_instrs(pnode, initial_live_set);

            //! NOTE: Remove phi in cur block lock from liveset
            //! TODO: Add into BasicBlock vector of PHI nodes
            for (auto &instr : block) {
                if (instr.opcode() == jj_vm::ir::Opcode::PHI)
                    initial_live_set.erase(&instr);
                if (is_empty_life_range(instr))
                    no_inputs_instrs.push_back(&instr);
            }

            //! NOTE: Process loops
            process_loop(pnode, initial_live_set);
        }

        //! NOTE: fiil the empty intervals
        for (auto *instr : no_inputs_instrs) {
            auto found_res = m_intervals.find(instr);
            auto &live_in = found_res->second;
            live_in.set_end(live_in.begin());
        }
    }

public:
    static LivenessAnalyzer<GraphTy> build(const GraphTy &graph) {
        LivenessBuilder<GraphTy> builder{graph};
        LivenessAnalyzer<GraphTy> analyzer{};
        //
        analyzer.m_intervals = std::move(builder.m_intervals);
        analyzer.m_live_sets = std::move(builder.m_live_sets);

        return analyzer;
    }
};

}  // namespace jj_vm::analysis::liveness
