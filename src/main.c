#include "ast.h"
#include "parser.h"
#include "stringstore.h"
#include "symboltable.h"
#include "util.h"
#include <stdio.h>
#include <core.h>


int main() {

    StringStore s = (StringStore) {
        .m = GlobalAllocator,
    };

    SymbolTable h = (SymbolTable) {
        .s = &s,
        .m = GlobalAllocator,
    };

    AST a = (AST) {
        .m = GlobalAllocator,
    };

    PushScope(&h);

    Tokenizer t = TokenLoadFile("tests/expr.csim", &s);
    printf("%.*s\n", (int)t.size, t.buffer);
    
    ParserState p = (ParserState) {
        .t = t,
        .s = &h,
        .a = &a,
    };
    
    Parse(&p, 0);

    //Debug
    PrintTable(&h);
    PrintAST(&a, &s);

    TokenUnloadFile(&t);

    return 0;
}
