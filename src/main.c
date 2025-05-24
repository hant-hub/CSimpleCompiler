#include "parser.h"
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

    Tokenizer t = TokenLoadFile("tests/text.csim", &s);
    printf("%.*s\n", (int)t.size, t.buffer);
    
    ParserState p = (ParserState) {
        .t = t
    };
    
    Parse(&p);



    TokenUnloadFile(&t);

    return 0;
}
