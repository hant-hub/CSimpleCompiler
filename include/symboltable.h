#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stringstore.h>
#include <ast.h>
#include <util.h>

/*
 * In C, traditionally they use four symbol tables.
 * One for variables, one for typedefs, one for functions,
 * and one for types.
 *
 * Rather than do that, instead I will instead have a single
 * symbol table with all data inside.
 *
 */

typedef enum SymbolType {
    SYMBOL_IDENTIFIER,
    SYMBOL_STRUCT,
} SymbolType;


//kinda like a tagged union but with
//indicies
typedef struct SymbolEntry {
    SymbolType type;
    ASTLogicalType l;
    u64 idx;
} SymbolEntry;

typedef struct HashTable {
    //allocator
    Allocator m;

    StringStore* s;

    //key table
    u64* keys;

    //values
    SymbolEntry* values;

    u64 size;
    u64 cap;
} HashTable;

typedef struct SymbolTable {
    Allocator m;
    StringStore* s;

    HashTable* tables;
    u64 size;
    u64 cap;

} SymbolTable;


u64 PushText(HashTable* h, char* text, u64 length);
i64 InsertEntry(HashTable* h, u64 key, SymbolEntry e);
i64 GetEntry(HashTable* h, u64 key);

void PushScope(SymbolTable* s);
void PopScope(SymbolTable* s);
i64 PushSymbol(SymbolTable* s, u64 key, SymbolEntry e);
SymbolEntry* GetSymbol(SymbolTable* s, u64 key);


void PrintTable(SymbolTable* s);

/*
 * Specific Tables
 *
 */

//Type Table -------------------------------
typedef struct TypeTable {
    Allocator m;
    //Later
} TypeTable;


// Variables -------------------------------
// More useful when structs and unions and typedefs
// are added
typedef struct VariableTable {
    Allocator m;

    u64* str;
    u64* typ; // index into type table
    u64 size;
    u64 cap;
} VariableTable;

u64 VariablePushEntry(VariableTable* t, u64 str, u64 typ);
void VariablePrint(VariableTable* v, StringStore* s);


#endif
