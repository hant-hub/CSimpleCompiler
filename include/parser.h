#ifndef PARSER_H
#define PARSER_H

#include <tokenizer.h>

typedef struct ParserState {
    Tokenizer t;
    Token curr_token;

} ParserState;

void Parse(ParserState* p);





#endif
