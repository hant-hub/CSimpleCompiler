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



Tokenizer TokenLoadFile(char* file, StringStore* s) {
    Tokenizer t = {
        .s = s,
    };

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

        return (Token) {
            .t = TOKEN_ID,
            .line = t->line,
            .val.i = idx,
        };
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

    char c = t->At[0];
    t->At++;

    //Literal
    return (Token) {
        .t = c,
        .line = t->line,
    };
}
