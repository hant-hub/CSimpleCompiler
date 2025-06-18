#ifndef UTIL_H
#define UTIL_H

#include "debug.h"
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef int8_t  i8;

typedef uint16_t u16;
typedef int16_t  i16;

typedef uint32_t u32;
typedef int32_t  i32;

typedef uint64_t u64;
typedef int64_t  i64;


#define AllocFunc(x) \
    void*(x)(u64 oldsize, u64 newsize, void* ptr, void* ctx)

typedef AllocFunc(*alloc_func);

//Allocator interface, for later
typedef struct Allocator {
    alloc_func alloc;
    void* ctx;
} Allocator;

Allocator MakeGlobalAllocator();

#define Alloc(a, size) a.alloc(0, size, NULL, a.ctx)
#define Free(a, ptr, size) a.alloc(size, 0, ptr, a.ctx)
#define Realloc(a, ptr, old, new) a.alloc(old, new, ptr, a.ctx)


//Currently using 32 bit, 4GB should
//be enough for now
typedef struct String {
    u8* bytes;
    u32 size;
} String;





#endif
