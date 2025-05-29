#ifndef PARSER_H
#define PARSER_H

#include <symboltable.h>
#include <tokenizer.h>

typedef struct ParserState {
    Tokenizer t;
    Token curr_token;

    AST* a;
    SymbolTable* s; //TODO(ELI): Implement declartions
} ParserState;

u32 Parse(ParserState* p, u32 num);





#endif
