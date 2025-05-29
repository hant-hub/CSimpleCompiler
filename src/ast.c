#include "stringstore.h"
#include <ast.h>
#include <stdio.h>
#include <stdlib.h>


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

    //variabel
    "BLOCK",

    //Quad
    "FOR",

    //Trinary
    "IF_ELSE",

    //Binary
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "LT",
    "GT",
    "EQ",
    "ASSIGN",

    //Unary
    "I2F",
    "F2I",

    //Leaf
    "FLOAT",
    "INT",
    "VAR",
};


const char* ASTGetName(ASTNodeType t) {
    return ASTNames[t - AST_BASE];
}


typedef struct ASTNodePair {
    u32 idx;
    u32 level; //indent level
} ASTNodePair;

typedef struct ASTNodeStack {
    ASTNodePair* stack;
    u32 size;
    u32 cap;
} ASTNodeStack;

static void PushNodePair(ASTNodeStack* s, u32 idx, u32 level) {
    if (s->size + 1 > s->cap) {
        s->cap = s->cap ? s->cap * 2 : 8;
        s->stack = realloc(s->stack, sizeof(ASTNodePair) * s->cap);
    }
    s->stack[s->size++] = (ASTNodePair) {
        .idx = idx,
        .level = level,
    };
}

static ASTNodePair PopNodePair(ASTNodeStack* s) {
    return s->stack[--s->size];
}

static void PrintTabs(u32 level) {
    printf("\t");
    for (u32 i = 0; i < level; i++) {
        printf("  ");
    }
}

void PrintAST(AST* a, StringStore* s) {
    printf("AST:\n");
    
    ASTNodeStack stack = {0};
    u32* levels = calloc(a->size, sizeof(u32));

    for (u32 i = 0; i < a->size; i++) {
        switch (a->nodes[i].t) {
            case AST_BASE: 
            {
                printf("Error, Invalid Node\n");
            } break;
            
            case AST_BLOCK:
            {
                u32 num = a->nodes[i].data.i;
                u32 level = 0;
                for (u32 j = 0; j < num; j++) {
                    u32 c = stack.stack[(stack.size - 1) - j].level;
                    level = c > level ? c : level;
                }
                
                for (u32 j = 0; j < num; j++) {
                    ASTNodePair p = PopNodePair(&stack);
                    levels[p.idx] = level;
                }

                PushNodePair(&stack, i, level + 1);

            } break;
            case AST_FOR:
            {
                ASTNodePair p1 = PopNodePair(&stack);
                ASTNodePair p2 = PopNodePair(&stack);
                ASTNodePair p3 = PopNodePair(&stack);
                ASTNodePair p4 = PopNodePair(&stack);

                u32 level = p1.level > p2.level ? p1.level : p2.level;
                level = p3.level > level ? p3.level : level;
                level = p4.level > level ? p4.level : level;

                levels[p1.idx] = level;
                levels[p2.idx] = level;
                levels[p3.idx] = level;
                levels[p4.idx] = level;

                PushNodePair(&stack, i, level + 1);

            } break;

            case AST_IF_ELSE:
            {
                ASTNodePair p1 = PopNodePair(&stack);
                ASTNodePair p2 = PopNodePair(&stack);
                ASTNodePair p3 = PopNodePair(&stack);

                u32 level = p1.level > p2.level ? p1.level : p2.level;
                level = p3.level > level ? p3.level : level;

                levels[p1.idx] = level;
                levels[p2.idx] = level;
                levels[p3.idx] = level;

                PushNodePair(&stack, i, level + 1);
            } break;

            case AST_SUB:
            case AST_MUL:
            case AST_DIV:
            case AST_LT:
            case AST_GT:
            case AST_EQ:
            case AST_ASSIGN:
            case AST_ADD:
            {
                ASTNodePair p1 = PopNodePair(&stack);
                ASTNodePair p2 = PopNodePair(&stack);

                u32 level = p1.level > p2.level ? p1.level : p2.level;

                levels[p1.idx] = level;
                levels[p2.idx] = level;

                PushNodePair(&stack, i, level + 1);
            } break;

            case AST_FLOAT_TO_INT:
            case AST_INT_TO_FLOAT:
            //case AST_STATEMENT:
            {
                ASTNodePair p = PopNodePair(&stack);

                levels[p.idx] = p.level;

                PushNodePair(&stack, i, p.level + 1);
            } break;
            case AST_FLOAT_CONST:
            {
                PushNodePair(&stack, i, 0);
                //printf("\t%s %f\n", ASTGetName(a->nodes[i].t), a->nodes[i].data.f);
            } break;
            case AST_INT_CONST:
            {
                PushNodePair(&stack, i, 0);
                //printf("\t%s %ld\n", ASTGetName(a->nodes[i].t), a->nodes[i].data.i);
            } break;
            case AST_VAR:
            {
                PushNodePair(&stack, i, 0);
                //printf("\t%s %s\n", ASTGetName(a->nodes[i].t), GetString(s, a->nodes[i].data.i));
            } break;
        }
    }

    while (stack.size) {
        ASTNodePair p = PopNodePair(&stack);
        levels[p.idx] = p.level;
    }

    for (u32 i = 0; i < a->size; i++) {
        PrintTabs(levels[i]);
        printf("%s\n", ASTGetName(a->nodes[i].t));
    }


    free(levels);
    free(stack.stack);
}
