#include "tokenizer.h"
#include <debug.h>
#include <stdio.h>


int main() {
    SetLogFile("test.log");
    Tokenizer t = LoadFile("tests/expr.csim");
    printf("%s\n", t.data);
    FreeFile(&t);
}
