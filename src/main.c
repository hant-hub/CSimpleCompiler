#include "parser.h"
#include "stringstore.h"
#include "symboltable.h"
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
    PushScope(&h);

    Tokenizer t = TokenLoadFile("tests/text.csim", &s);
    printf("%.*s\n", (int)t.size, t.buffer);
    
    ParserState p = (ParserState) {
        .t = t,
        .s = &h,
    };
    
    Parse(&p);

    printf("SymbolTable:\n");
    for (u32 i = 0; i < h.tables[0].cap; i++) {
        if (h.tables[0].keys[i] != -1) {
            printf("\t%s\n", GetString(&s, h.tables[0].keys[i]));
        } else {
            printf("\tEmpty\n");
        }
    }

    TokenUnloadFile(&t);

    return 0;
}
