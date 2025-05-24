#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "stringstore.h"
#include <symboltable.h>
#include <util.h>


typedef enum TokenType {
    TOKEN_INVALID = 0, //NULL
    TOKEN_ID = 128, //room for literals

    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,

    TOKEN_NUM_INT,
    TOKEN_NUM_FLOAT,

    TOKEN_EQ,

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

const char* GetTokenTypeName(TokenType t);
const char* GetTokenName(StringStore* s, Token t);

#endif
