#include "stringstore.h"
#include <stdio.h>
#include <string.h>
#include <symboltable.h>

void Resize(HashTable* h) {
    SymbolEntry* oldvals = h->values; 
    u64* oldkeys = h->keys;
    u64 oldsize = h->cap;


    while ((f32)(h->size + 1)/(f32)h->cap > 0.6) 
        h->cap = h->cap ? h->cap * 2: 1;

    //printf("Resize: %ld %ld\n", oldsize, h->cap);
    h->keys = h->m.a(oldsize * sizeof(u64), h->cap * sizeof(u64), NULL, h->m.ctx);
    h->values = h->m.a(oldsize * sizeof(SymbolEntry), h->cap * sizeof(SymbolEntry), NULL, h->m.ctx);
    memset(h->keys, -1, h->cap * sizeof(u64));


    for (u64 i = 0; i < oldsize; i++) {
        if (oldkeys[i] == -1) continue;

        char* key = GetString(h->s, oldkeys[i]);
        u64 idx = hash(key, strlen(key))  % h->cap;

        u64 k = oldkeys[i];
        SymbolEntry e = oldvals[i];

        for (u32 i = 0; i < h->cap; i++) {
            if (h->keys[idx] == -1) {
                h->values[idx] = e;
                h->keys[idx] = k;
                break;
            }
        }
        idx = (idx + 1) & ~h->cap;
    }

    h->m.a(oldsize * sizeof(u64), 0, oldkeys, h->m.ctx);
    h->m.a(oldsize * sizeof(SymbolEntry), 0, oldvals, h->m.ctx);
}

i64 InsertEntry(HashTable* h, u64 kidx, SymbolEntry e) {
    //resize if necessary
    //printf("Check: %f, %f\n", (f32)(h->size + 1), (f32)h->cap); 
    while ((f32)(h->size + 1)/(f32)h->cap > 0.6) {
        //printf("Resize!\n");
        Resize(h);
    }

    char* key = GetString(h->s, kidx);
    u64 length = strlen(key);
    
    //printf("Insert %s\n", key);

    //traverse and find slot
    i64 idx = hash(key, length) % h->cap;

    for (u32 i = 0; i < h->cap; i++) {
        if (h->keys[idx] == -1) {
            h->values[idx] = e;
            h->keys[idx] = kidx;
            h->size++;
            break;
        }

        if (h->keys[i] == kidx) {
            //h->values[idx] = e;
            idx *= -1;
            break;
        }

        idx = (idx + 1) & ~h->cap;
    }

    return idx; 
}


i64 GetEntry(HashTable* h, u64 kidx) {
    if (kidx == -1) return -1;
    char* key = GetString(h->s, kidx);
    u64 length = strlen(key);

    i64 idx = hash(key, length) % h->cap;
    for (u32 i = 0; i < h->cap; i++) {
        if (h->keys[idx] < 0) {
            idx = -1;
            break;
        }

        if (h->keys[idx] == kidx) {
            break;
        }
    }


    return idx;
}


void PushScope(SymbolTable* s) {
    if (s->size + 1 > s->cap) {
        u64 oldsize = s->cap;
        s->cap = s->cap ? s->cap * 2 : 1;
        s->tables = s->m.a(
                oldsize * sizeof(HashTable),
                s->cap * sizeof(HashTable),
                s->tables,
                s->m.ctx);

        s->tables[s->size++] = (HashTable) {
            .m = s->m,
            .s = s->s,
        };

    } else {

        s->tables[s->size].size = 0;
        memset(s->tables[s->size].keys, -1,
               sizeof(u64) * s->tables[s->size].cap);
    }
}

void PopScope(SymbolTable* s) {
    //clear top level scope
    //the table is delibrately preserved
    //to avoid redundant allocations and frees

    s->size--;
    s->tables[s->size].size = 0;
    memset(s->tables[s->size].keys, -1,
            sizeof(u64) * s->tables[s->size].cap);

}

void PushSymbol(SymbolTable* s, u64 key, SymbolEntry e) {
    InsertEntry(&s->tables[s->size - 1], key, e);
}

SymbolEntry* GetSymbol(SymbolTable* s, u64 key) {
    for (u64 i = 0; i < s->size; i++) {
        u64 idx = s->size - i - 1;
        i64 r = GetEntry(&s->tables[idx], key);
        if (r > 0) {
            return &s->tables[idx].values[r];
        }
    }
    return NULL;
}
