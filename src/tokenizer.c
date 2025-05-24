#include "stringstore.h"
#include "symboltable.h"
#include "util.h"
#include <tokenizer.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/mman.h>

typedef struct Keyword{
    u64 key;
    TokenType t;
} Keyword;

u32 keywordsInitialized = 0;
Keyword Keywords[] = {
    (Keyword){.t = TOKEN_INT},
    (Keyword){.t = TOKEN_FLOAT},
    (Keyword){.t = TOKEN_IF},
    (Keyword){.t = TOKEN_ELSE},
    (Keyword){.t = TOKEN_FOR},
};



Tokenizer TokenLoadFile(char* file, StringStore* s) {
    Tokenizer t = {
        .s = s,
    };

    if (!keywordsInitialized) {
        keywordsInitialized = 1;

        Keywords[0].key = AddString(s, "int", 3);
        printf("Keyword: %s\n", GetString(s, Keywords[0].key)); 
        Keywords[1].key = AddString(s, "float", 5);
        printf("Keyword: %s\n", GetString(s, Keywords[1].key)); 
        Keywords[2].key = AddString(s, "if", 2);
        printf("Keyword: %s\n", GetString(s, Keywords[2].key)); 
        Keywords[3].key = AddString(s, "else", 4);
        printf("Keyword: %s\n", GetString(s, Keywords[3].key)); 
        Keywords[4].key = AddString(s, "for", 3);
        printf("Keyword: %s\n", GetString(s, Keywords[4].key)); 
    }

    int fd = open(file, O_RDONLY);
    if (!fd) {
        dprintf(STDERR_FILENO, "Faild to open %s\n", file);
        exit(-1);
    }

    struct stat info;
    if (fstat(fd, &info)) {
        dprintf(STDERR_FILENO, "Faild to stat %s\n", file);
        exit(-1);
    }

    t.buffer = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    t.size = info.st_size;
    t.At = t.buffer;

    return t;
}

void TokenUnloadFile(Tokenizer* t) {
    munmap(t->buffer, t->size);
}

u64 ConvertInt(char* s, u64 len) {

    u64 v = 0;
    for (u64 i = 0; i < len; i++) {
        v *= 10;
        v += s[i] - '0';
    }

    return v;
}

f64 ConvertFloat(char* integer, u64 ilen, char* dec, u64 dlen) {

    f64 w = 0;
    for (u64 i = 0; i < ilen; i++) {
        w *= 10;
        w += integer[i] - '0';
    }

    f64 n = 0;
    f64 d = 1;
    for (u64 i = 0; i < dlen; i++) {
        d *= 10;
        n *= 10;
        n += dec[i] - '0';
    }

    return w + (n/d);
}


void EatWhiteSpace(Tokenizer* t) {
    while (1) {
        if (t->At - t->buffer > t->size) return;
        if (t->At[0] != '\r' && 
            t->At[0] != '\t' && 
            t->At[0] != '\n' &&
            t->At[0] != ' ') {
            return;
        }

        if (t->At[0] == '\n') t->line++;
        t->At++;
    }
}

Token EatToken(Tokenizer* t) {
    EatWhiteSpace(t);
    if (t->At - t->buffer > t->size) {
        return (Token) {
            .t = TOKEN_EOF,
        };
    }

    if (isalpha(t->At[0]) || t->At[0] == '_') {
        //ID
        char* start = t->At;
        while (isalnum(t->At[0]) || t->At[0] == '_') {
            t->At++;
        }
        u32 len = t->At - start;

        i64 idx = AddString(t->s, start, len); 
        if (idx < 0) idx *= -1;

        Token out = (Token) {
            .t = TOKEN_ID,
            .line = t->line,
            .val.i = idx,
        };

        for (u32 i = 0; i < (sizeof(Keywords)/sizeof(Keywords[0])); i++) {
            if (idx == Keywords[i].key) {
                out.t = Keywords[i].t;
                break;
            }
        }

        return out;
    }

    if (isdigit(t->At[0])) {
        //Num
        char* start = t->At;
        while (isdigit(t->At[0])) {
            t->At++;
        }
        if (t->At[0] == '.') {
            //floats

            u32 intlen = t->At - start;
            char* dec = ++t->At;

            while (isdigit(t->At[0])) {
                t->At++;
            }
            u32 declen = t->At - dec;

            return (Token) {
                .t = TOKEN_NUM_FLOAT,
                .line = t->line,
                .val.f = ConvertFloat(start, intlen, dec, declen),
            };
        }
        //ints


        u32 len = t->At - start;

        return (Token) {
            .t = TOKEN_NUM_INT,
            .line = t->line,
            .val.i = ConvertInt(start, len),
        };
    }

    if (t->At[0] == '=' && t->At[1] == '=') {
        t->At += 2;
        return (Token) {
            .t = TOKEN_EQ,
            .line = t->line,
        };
    }


    char c = t->At[0];
    t->At++;

    //Literal
    return (Token) {
        .t = c,
        .line = t->line,
    };
}

static const char* TokenNames[] = {
    //TOKEN_INVALID,
    "TOKEN_ID",
    "TOKEN_INT",
    "TOKEN_FLOAT",
    "TOKEN_IF",
    "TOKEN_ELSE",
    "TOKEN_FOR",
    "TOKEN_NUM_INT",
    "TOKEN_NUM_FLOAT",
    "TOKEN_EQ",
    "TOKEN_EOF",
};


const char* GetTokenTypeName(TokenType t) {
    static char tmpbuf[2] = {0}; //temporary buffer
    switch (t) {
        case TOKEN_INVALID:
        {
            return "INVALID";
        } break;
        case TOKEN_ID:
        case TOKEN_INT:
        case TOKEN_FLOAT:
        case TOKEN_IF:
        case TOKEN_ELSE:
        case TOKEN_FOR:
        case TOKEN_NUM_INT:
        case TOKEN_NUM_FLOAT:
        case TOKEN_EQ:
        case TOKEN_EOF:
        {
            return TokenNames[t - TOKEN_ID];
        } break;
        default:
        {
            tmpbuf[0] = t;
            return tmpbuf;
        } break;
    }
}

const char* GetTokenName(StringStore* s, Token t) {
    static char tmpbuf[2] = {0};
    if (t.t < 128) {
        tmpbuf[0] = t.t;
        return tmpbuf;
    }
    switch (t.t) {
        case TOKEN_ID:
        case TOKEN_INT:
        case TOKEN_FLOAT:
        case TOKEN_IF:
        case TOKEN_ELSE:
        case TOKEN_FOR:
        {
            return GetString(s, t.val.i);
        }
        case TOKEN_NUM_INT:
        case TOKEN_NUM_FLOAT:
        {
            return "NUM";
        }

        case TOKEN_EQ:
        {
            return "EQ";
        }
        default:
        {
        }
    }
    return NULL;
}
