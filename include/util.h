#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

//Memory allocators

#define AllocFunc(x) \
    void* (x) (u64 oldsize, u64 newsize, void* ptr, void* ctx)

typedef AllocFunc(*AllocatorFunction);


typedef struct Allocator {
    AllocatorFunction a;
    void* ctx;
} Allocator;

extern Allocator GlobalAllocator;

//FNV 1a
static u64 hash(char* data, u64 len) {
    u64 hash = 14695981039346656037UL;
    for (u64 i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 1099511628211UL;
    }

    return hash;
}

#endif
