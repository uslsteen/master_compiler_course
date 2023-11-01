# Java Junior IR 

There are Java Junior Intermidiate Representation library 

- [opcodes.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/opcodes.hh) - simple defenition of JJ IR instructions opcodes

- [instruction.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/instruction.hh) - implementation of a base instruction class that defines the basic interface of any instruction.
The basic instruction inherits:
    - ```intrusive node``` class due implemenntation into instrusive list
    - ```Value ``` class for the ability to determine the data types with which the instruction operates

- [instructions.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/instructions.hh) - implementation of different direved JJ IR instructions 

- [basic_block.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/basic_block.hh) - implementation linear code, which should ended instruction-terminator. By definition it has two successors and many predecessors

- [funcion.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/function.hh) - implementation of set of executed basic blocks. By definition function has one return type and many input parameters, which also has a types.

- [ir_builder.hh](https://github.com/uslsteen/master_compiler_course/blob/main/include/IR/ir_builder.hh) - implementation of user interface to create any instruction in the current basic block