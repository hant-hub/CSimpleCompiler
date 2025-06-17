# CSimpleCompiler

# Compilation and Usage
Currently the project is in very early development
and parses hardcoded files. 

To Compile simply compile sb_build.c. Then run the resulting
executable in the project source directory. This will create a build
folder, compile_commands.json, and run the compiler.

Currently only Linux is supported.

> This is a temporary version, will be rewritten from scratch to use
a simpler AST data structures

# TODO:
- [ ] Tokenizer
- [ ] Logging
- [ ] General Purpose HashMap
    - [ ] String Interning
    - [ ] Perfect Hash Map support?
- [ ] Parser
    - [ ] Recursive Descent
    - [ ] Expression Parsing
        - [ ] Maybe check out Pratt Parsing
        - [ ] Shunting Yard (classic)
        - [ ] Precedence Climbing
        - [ ] Double-E infix
- [ ] AST
    - [ ] AST Interface
    - [ ] Linked AST
    - [ ] Flat AST
    - [ ] Printing Tools
- [ ] CodeGen
    - [ ] Binary IR
    - [ ] LLVM IR
- [ ] Language Design
    - [ ] Grammer rework
        - [ ] More like C, with rvalues and lvalues
    - [ ] Function Grammar
    - [ ] More Operators
    - [ ] Reflection?
    - [ ] Lambdas
    - [ ] More Types
    - [ ] Structs, unions, and arrays
