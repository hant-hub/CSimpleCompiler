#ifndef AST_H
#define AST_H

#include <util.h>

//Programmer facing type system
typedef enum ASTLogicalType {
    AST_L_UNDEFINED, //used before type inference
    AST_L_FLOAT,
    AST_L_INT,
} ASTLogicalType;

//Used to determine how many
//Children
typedef enum ASTNodeType {
    //Binary ops
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_LT,
    AST_EQ,

    //Unary ops
    AST_INT_TO_FLOAT,
    AST_FLOAT_TO_INT,

    //Leaf Nodes
    AST_FLOAT_CONST,
    AST_INT_CONST,
    AST_VAR,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType t;
    ASTLogicalType l; 

    union {
        //Note: Both values are the same size, so there isn't
        //any wasted space to 'union' shennanigans.
        
        u64 i; //index for var, or raw value for constant
        f64 f; //raw value for constant
    } data;
} ASTNode;

//Tree is stored in linearized post order
//This is possible since we use recursive descent
//and the post order is implicit if we first parse,
//then push our node onto the tree.
//
//By traversing with a stack the children will be
//implicit since we know how many children each node
//should have.
typedef struct AST {
    Allocator m;

    ASTNode* nodes;
    u32 size;
    u32 cap;
} AST;


#endif
