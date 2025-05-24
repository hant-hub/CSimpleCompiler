#include "stringstore.h"
#include "tokenizer.h"
#include <parser.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

//utils
static u32 _TokenInSet(Token t, u32 num, ...) {
    va_list args;
    va_start(args, num);
    for (u32 i = 0; i < num; i++) {
        TokenType type = va_arg(args, TokenType);
        if (t.t == type) {
            return 1;
        }
    }
    va_end(args);

    return 0;
}

static u32 _ThrowError(ParserState* p, Token t, TokenType expected, u32 line) {
    fprintf(stderr, "Error On line: %ld\n", t.line + 1);
    fprintf(stderr, "Parser Line: %d\n", line);
    fprintf(stderr, "Expected Token: %s, Got %s\n", GetTokenTypeName(expected), GetTokenName(p->t.s, t));
    exit(-1);
}

#define ThrowError(p, t, e) \
    _ThrowError(p, t, e, __LINE__)

static void _RequireToken(ParserState* p, TokenType type, u32 line) {
    if (p->curr_token.t != type) {
        _ThrowError(p, p->curr_token, type, line);
    }
    p->curr_token = EatToken(&p->t);
}

#define RequireToken(p, t) \
    _RequireToken(p, t, __LINE__);

#define TokenInSet(t, ...) \
    _TokenInSet(t, NUMARGS(__VA_ARGS__), __VA_ARGS__)


//non terminals
void ParseStatement(ParserState* p);
void ParseDeclaration(ParserState* p);
void ParseAssignment(ParserState* p);
void ParseExpr(ParserState* p);
void ParseComp(ParserState* p);
void ParseFactor(ParserState* p);
void ParseUnit(ParserState* p);

void ParseIf(ParserState* p);
void ParseBlock(ParserState* p);
void ParseFor(ParserState* p);


void Parse(ParserState* p) {
    if (p->curr_token.t == TOKEN_INVALID)
        p->curr_token = EatToken(&p->t);

    if (p->curr_token.t == '}') return;
    if (p->curr_token.t != TOKEN_EOF){
        ParseStatement(p);
        Parse(p);
    }

    return;
}


void ParseStatement(ParserState* p) {

    if (TokenInSet(p->curr_token, TOKEN_INT, TOKEN_FLOAT)) {
        printf("declaration\n");
        ParseDeclaration(p);
        return;
    }

    if (TokenInSet(p->curr_token, TOKEN_ID)) {
        printf("assignment: %s\n", GetTokenName(p->t.s, p->curr_token));
        ParseAssignment(p);
        RequireToken(p, ';');
        return;
    }

    if (TokenInSet(p->curr_token, TOKEN_IF)) {
        printf("if else\n");
        ParseIf(p);
        return;
    }

    if (TokenInSet(p->curr_token, '{')) {
        printf("block\n");
        ParseBlock(p);
        return;
    }

    if (TokenInSet(p->curr_token, TOKEN_FOR)) {
        printf("for\n");
        ParseFor(p);
        return;
    }


    fprintf(stderr, "Error On line: %ld\n", p->curr_token.line + 1);
    fprintf(stderr, "Expected Token: ID or IF or FOR or FLOAT or INT, Got \"%s\"\n", GetTokenName(p->t.s, p->curr_token));
    exit(-1);
}

void ParseDeclaration(ParserState* p) {
    p->curr_token = EatToken(&p->t); //process type
    printf("Newvar: %s\n", GetTokenName(p->t.s, p->curr_token));
    RequireToken(p, TOKEN_ID);
    RequireToken(p, ';');
}

void ParseAssignment(ParserState* p) {
    p->curr_token = EatToken(&p->t); //process id
    RequireToken(p, '=');
    ParseExpr(p);
}

void ParseExpr(ParserState* p) {
    ParseComp(p);
    if (TokenInSet(p->curr_token, '<', '>', TOKEN_EQ)) {
        p->curr_token = EatToken(&p->t);
        ParseExpr(p);
    }
}

void ParseComp(ParserState* p) {
    ParseFactor(p);
    if (TokenInSet(p->curr_token, '+', '-')) {
        p->curr_token = EatToken(&p->t);
        ParseComp(p);
    }
}

void ParseFactor(ParserState* p) {
    ParseUnit(p);
    if (TokenInSet(p->curr_token, '*', '/')) {
        p->curr_token = EatToken(&p->t);
        ParseFactor(p);
    }
}

void ParseUnit(ParserState* p) {
    switch ((u32)p->curr_token.t) {
        case '(':
        {
            p->curr_token = EatToken(&p->t);
            ParseExpr(p);
            RequireToken(p, ')');
            return;
        };
        case TOKEN_NUM_FLOAT:
        case TOKEN_NUM_INT:
        case TOKEN_ID:
        {
            p->curr_token = EatToken(&p->t);
            return;
        };
        default:
        {
            ThrowError(p, p->curr_token, TOKEN_ID);
        };
    }
}


void ParseIf(ParserState* p) {
    RequireToken(p, TOKEN_IF);
    RequireToken(p, '(');
    ParseExpr(p);
    RequireToken(p, ')');
    ParseBlock(p);
    RequireToken(p, TOKEN_ELSE);
    ParseBlock(p);
}

void ParseBlock(ParserState* p) {
    if (p->curr_token.t == '{') {
        p->curr_token = EatToken(&p->t);
        Parse(p);
        RequireToken(p, '}');
        return;
    }

    ParseStatement(p);
}
void ParseFor(ParserState* p) {

    RequireToken(p, TOKEN_FOR);
    RequireToken(p, '(');
    ParseAssignment(p);
    RequireToken(p, ';');

    ParseExpr(p);
    RequireToken(p, ';');

    ParseAssignment(p);

    RequireToken(p, ')');

    ParseBlock(p);
}

