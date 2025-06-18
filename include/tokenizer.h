#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "util.h"

typedef enum TokenType : u32 {
    //literals
    
    //Generic stuff
    TOKEN_ID = 128,
    TOKEN_CONSTF,
    TOKEN_CONSTI,

    //Keywords
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_IF,
    TOKEN_FOR,

    //Multi Character Operators
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LEQ,
    TOKEN_GEQ,

    //Special
    TOKEN_EOF,
} TokenType;

typedef struct Tokenizer {
    Allocator a;
    u8* data;
    u8* At;
    u64 size;
} Tokenizer;

typedef struct Token {
    TokenType type;
    String string; //temporary 
} Token;

Tokenizer LoadFile(const char* str);
void FreeFile(Tokenizer* t);

//Create Offset Version with circular buffer
Token GetToken(Tokenizer* t);








#endif
