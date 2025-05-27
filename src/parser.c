#include "ast.h"
#include "stringstore.h"
#include "symboltable.h"
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

static void _TokenThrowError(ParserState* p, Token t, TokenType expected, u32 line) {
    fprintf(stderr, "Error On line: %ld\n", t.line + 1);
    fprintf(stderr, "Parser Line: %d\n", line);
    fprintf(stderr, "Expected Token: %s, Got %s\n", GetTokenTypeName(expected), GetTokenName(p->t.s, t));
    exit(-1);
}

static void _SymbolThrowError(ParserState* p, Token t, u32 line) {
    fprintf(stderr, "Error On line: %ld\n", t.line + 1);
    fprintf(stderr, "Parser Line: %d\n", line);
    fprintf(stderr, "Invalid Reference: %s\n", GetTokenName(p->t.s, t));
    exit(-1);
}

#define TokenThrowError(p, t, e) \
    _TokenThrowError(p, t, e, __LINE__)

#define SymbolThrowError(p, t) \
    _SymbolThrowError(p, t, __LINE__)

static void _RequireToken(ParserState* p, TokenType type, u32 line) {
    if (p->curr_token.t != type) {
        _TokenThrowError(p, p->curr_token, type, line);
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
    SymbolEntry e = (SymbolEntry) {
        .idx = -1, //Temporary
        .type = SYMBOL_IDENTIFIER,
    };

    if (p->curr_token.t == TOKEN_INT) {
        e.l = AST_L_INT;
    } else {
        e.l = AST_L_FLOAT;
    }

    p->curr_token = EatToken(&p->t); //process type
    printf("Newvar: %s\n", GetTokenName(p->t.s, p->curr_token));


    if (PushSymbol(p->s, p->curr_token.val.i, e) < 0) {
        SymbolThrowError(p, p->curr_token);
    }

    RequireToken(p, TOKEN_ID);
    RequireToken(p, ';');
}

void ParseAssignment(ParserState* p) {
    ASTNode var = {.t = AST_VAR};
    SymbolEntry* e = GetSymbol(p->s, p->curr_token.val.i);

    if (!e) {
        SymbolThrowError(p, p->curr_token);
    }
    var.data.i = p->curr_token.val.i;
    ASTPushNode(p->a, var);

    p->curr_token = EatToken(&p->t); //process id
    RequireToken(p, '=');
    ParseExpr(p);

    ASTNode a = {.t = AST_ASSIGN};
    ASTPushNode(p->a, a);


}

void ParseExpr(ParserState* p) {
    ParseComp(p);
    if (TokenInSet(p->curr_token, '<', '>', TOKEN_EQ)) {
        ASTNode n;
        if (p->curr_token.t == '<') {
            n = (ASTNode){
                .t = AST_LT,
                .l = AST_L_UNDEFINED,
            };
        } else if (p->curr_token.t == '>') {
            n = (ASTNode){
                .t = AST_GT,
                .l = AST_L_UNDEFINED,
            };
        } else {
            n = (ASTNode){
                .t = AST_EQ,
                .l = AST_L_UNDEFINED,
            };
        }

        p->curr_token = EatToken(&p->t);
        ParseExpr(p);
        ASTPushNode(p->a, n);
    }
}

void ParseComp(ParserState* p) {
    ParseFactor(p);
    if (TokenInSet(p->curr_token, '+', '-')) {
        ASTNode n;
        if (p->curr_token.t == '+') {
            n = (ASTNode){
                .t = AST_ADD,
                .l = AST_L_UNDEFINED,
            };
        } else {
            n = (ASTNode){
                .t = AST_SUB,
                .l = AST_L_UNDEFINED,
            };
        }
        p->curr_token = EatToken(&p->t);
        ParseComp(p);
        ASTPushNode(p->a, n);
    }
}

void ParseFactor(ParserState* p) {
    ParseUnit(p);
    if (TokenInSet(p->curr_token, '*', '/')) {
        ASTNode n;
        if (p->curr_token.t == '*') {
            n = (ASTNode){
                .t = AST_MUL,
                .l = AST_L_UNDEFINED,
            };
        } else {
            n = (ASTNode){
                .t = AST_DIV,
                .l = AST_L_UNDEFINED,
            };
        }

        p->curr_token = EatToken(&p->t);
        ParseFactor(p);
        ASTPushNode(p->a, n);
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
        {
            ASTNode n = (ASTNode) {
                .t = AST_FLOAT_CONST,
                .l = AST_L_FLOAT,
                .data.f = p->curr_token.val.f
            };
            ASTPushNode(p->a, n);
            p->curr_token = EatToken(&p->t);
            return;
        };
        case TOKEN_NUM_INT:
        {
            ASTNode n = (ASTNode) {
                .t = AST_INT_CONST,
                .l = AST_L_INT,
                .data.i = p->curr_token.val.i
            };
            ASTPushNode(p->a, n);
            p->curr_token = EatToken(&p->t);
            return;
        };
        case TOKEN_ID:
        {
            SymbolEntry* e = GetSymbol(p->s, p->curr_token.val.i);
            if (!e) SymbolThrowError(p, p->curr_token);

            ASTNode n = (ASTNode) {
                .t = AST_VAR,
                .l = e->l,
                .data.i = p->curr_token.val.i,
            };

            ASTPushNode(p->a, n);
            p->curr_token = EatToken(&p->t);
            return;
        };
        default:
        {
            TokenThrowError(p, p->curr_token, TOKEN_ID);
        };
    }
}


void ParseIf(ParserState* p) {
    ASTNode n = (ASTNode){.t = AST_IF_ELSE};
    RequireToken(p, TOKEN_IF);
    RequireToken(p, '(');
    ParseExpr(p);
    RequireToken(p, ')');
    ParseBlock(p);
    RequireToken(p, TOKEN_ELSE);
    ParseBlock(p);
    ASTPushNode(p->a, n);
}

void ParseBlock(ParserState* p) {
    if (p->curr_token.t == '{') {
        PushScope(p->s);
        p->curr_token = EatToken(&p->t);
        Parse(p);
        RequireToken(p, '}');
        PopScope(p->s);
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

    ASTPushNode(p->a, (ASTNode) {.t = AST_FOR});

}

