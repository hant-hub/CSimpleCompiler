#include "stringstore.h"
#include "symboltable.h"
#include "tokenizer.h"
#include "util.h"
#include <stdio.h>
#include <core.h>


int main() {

    StringStore s = (StringStore) {
        .m = GlobalAllocator,
    };

    HashTable h = (HashTable) {
        .s = &s,
        .m = GlobalAllocator,
    };

    Tokenizer t = TokenLoadFile("tests/text.csim", &s);
    printf("%.*s\n", (int)t.size, t.buffer);
    
    Token tok;
    while ((tok = EatToken(&t)).t != TOKEN_EOF) {
        if (tok.t == TOKEN_ID) {
            u64 sdx = t.s->keys[tok.val.i];
            char* s = &t.s->textbuffer[sdx];
            printf("%s\n", s);

        }
        else if (tok.t == TOKEN_NUM_INT) printf("%ld\n", tok.val.i);
        else if (tok.t == TOKEN_NUM_FLOAT) printf("%f\n", tok.val.f);
        else printf("%c\n", tok.t);
    }

    printf("stringstore\n");
    for (u64 i = 0; i < s.kcap; i++) {
        if (s.keys[i] != -1) {
            printf("\t%s\n", &s.textbuffer[s.keys[i]]);
            InsertEntry(&h, s.keys[i], (SymbolEntry){});
        }
        else printf("\tEMPTY\n");
    }

    printf("hashtable\n");
    for (u64 i = 0; i < h.cap; i++) {
        if (h.keys[i] != -1) {
            printf("\t%s\n", &s.textbuffer[h.keys[i]]);
        }
        else printf("\tEMPTY\n");
    }
    
    printf("String: %ld\n", s.ksize);


    TokenUnloadFile(&t);

    return 0;
}
