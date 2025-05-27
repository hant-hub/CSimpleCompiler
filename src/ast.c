#include "stringstore.h"
#include <ast.h>
#include <stdio.h>


void ASTPushNode(AST* a, ASTNode n) {
    if (a->size + 1 > a->cap) {
        u32 oldsize = a->cap;
        a->cap = a->cap ? a->cap * 2 : 4;
        a->nodes = a->m.a(
                oldsize * sizeof(ASTNode),
                a->cap * sizeof(ASTNode),
                a->nodes,
                a->m.ctx
                );
    }
    a->nodes[a->size++] = n;
}

static const char* ASTNames[] = {
    "BASE", //Should never be used

    "IF_ELSE",

    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "LT",
    "GT",
    "EQ",
    "ASSIGN",

    "I2F",
    "F2I",

    "FLOAT CONST",
    "INT CONST",
    "VAR",
};


const char* ASTGetName(ASTNodeType t) {
    return ASTNames[t - AST_BASE];
}

void PrintAST(AST* a, StringStore* s) {
    printf("AST:\n");
    for (u32 i = 0; i < a->size; i++) {
        switch (a->nodes[i].t) {
            case AST_INT_CONST:
            {
                printf("\t%s %ld\n", ASTGetName(a->nodes[i].t), a->nodes[i].data.i);
            } break;
            case AST_VAR:
            {
                printf("\t%s %s\n", ASTGetName(a->nodes[i].t), GetString(s, a->nodes[i].data.i));
            } break;
            default:
            {
                printf("\t%s\n", ASTGetName(a->nodes[i].t));
            } break;
        }
    }
}
