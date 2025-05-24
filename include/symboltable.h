#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "stringstore.h"
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
void PushSymbol(SymbolTable* s, u64 key, SymbolEntry e);
SymbolEntry* GetSymbol(SymbolTable* s, u64 key);

#endif
