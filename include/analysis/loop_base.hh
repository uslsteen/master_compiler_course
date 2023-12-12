#pragma once

namespace jj_vm::analysis::loop {
//

template <class GraphTy>
class Loop3NodeBase {

    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;
};

}