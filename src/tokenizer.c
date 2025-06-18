#include "util.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <tokenizer.h>

Tokenizer LoadFile(const char* str) {
    Tokenizer t = {0};
    FILE* f = fopen(str, "r");
    
    struct stat info;
    stat(str, &info);

    t.a = MakeGlobalAllocator();
    t.size = info.st_size;
    t.data = Alloc(t.a, t.size + 1);
    memset(t.data, 0, t.size + 1);

    fread(t.data, t.size, 1, f); 

    fclose(f);

    return t;
}

void FreeFile(Tokenizer* t) {
    Free(t->a, t->data, t->size + 1); 
}


Token GetToken(Tokenizer* t) {



    return (Token) {
        .type = t->At++[0],
        .string = {0}
    };
}
