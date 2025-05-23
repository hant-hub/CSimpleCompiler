#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "stringstore.h"
#include <symboltable.h>
#include <util.h>


typedef enum TokenType {
    TOKEN_ID,
    TOKEN_NUM_INT,
    TOKEN_NUM_FLOAT,
    TOKEN_EOF,
} TokenType;

typedef struct Token {
    TokenType t;
    u64 line;

    //Index for ids,
    //Raw value for nums

    union {
        u64 i; //ID and Num
        f64 f; //Num
    } val;

} Token;

typedef struct Tokenizer {
    StringStore* s;

    char* buffer;
    u64 size;

    char* At;
    u64 line;
} Tokenizer;


Tokenizer TokenLoadFile(char* file, StringStore* s);
void TokenUnloadFile(Tokenizer* t);

Token EatToken(Tokenizer* t);

#endif
