//! NOTE:                This relative path exists for reason
//!       get simple include and dont provide include directories in cmake file
//                            *** DONT CHANGE IT ***

#include "../graph/builder.hh"
//
#include <gtest/gtest.h>
#include <sys/types.h>

namespace jj_vm::testing {
//

class LivenessTest1 : public LivenessInterface {
protected:
    LivenessTest1() = default;

    void create_test() {
        init_test(4);

        //

        build();
    }
};

}
