#  Master compiler course

Compiler optimizations course, 9th semestr, MIPT

- [Java Junior LLVM-like Intermediate Representation](https://github.com/uslsteen/master_compiler_course/tree/main/include/IR) 


### Assignment 1, IR
- Implementation of [basic IR](https://github.com/uslsteen/master_compiler_course/tree/main/include/IR) & [instrusive list](https://github.com/uslsteen/master_compiler_course/tree/main/include/intrusive_list)
- [Unit test](https://github.com/uslsteen/master_compiler_course/blob/main/tests/unit/IR/IR.cc) to check correctness of builded IR and other entities

### Assignment 2, DFS, Dominator Tree implememtation
- Implementation of [deep first search](https://github.com/uslsteen/master_compiler_course/blob/main/include/graph/dfs.hh) & [dominator tree builder](https://github.com/uslsteen/master_compiler_course/blob/main/include/graph/dom3.hh) algorithms
- [Unit test](https://github.com/uslsteen/master_compiler_course/tree/main/tests/unit/graph) to check correctness of [run DFS](https://github.com/uslsteen/master_compiler_course/blob/main/tests/unit/graph/dfs.cc) and [builded dominator tree](https://github.com/uslsteen/master_compiler_course/blob/main/tests/unit/graph/dom3.cc)

### Assignment 3 Loop Analyser
- Implementation of [basic Loop Analyser](https://github.com/uslsteen/master_compiler_course/blob/main/include/analysis/loop_analyzer.hh). It builds Loop Tree using Deep First Search algorithm and domimatee properties of nodes.
- [Unit test](https://github.com/uslsteen/master_compiler_course/blob/main/tests/unit/analysis/loop_analyzer.cc) to check correctness of builded Loop Tree.

