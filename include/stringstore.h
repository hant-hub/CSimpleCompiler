#ifndef STRING_STORE_H
#define STRING_STORE_H

#include <util.h>

typedef struct StringStore {
    Allocator m;

    char* textbuffer; 
    u64 tsize;
    u64 tcap;

    u64* keys;
    u64 ksize;
    u64 kcap;
} StringStore;

i64 AddString(StringStore* h, char* key, u64 length);
char* GetString(StringStore* h, int key);



#endif
