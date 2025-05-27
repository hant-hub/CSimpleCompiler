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

void Parse(ParserState* p);





#endif
