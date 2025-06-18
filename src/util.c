#include "debug.h"
#include <stdlib.h>
#include <util.h>
#include <assert.h>

static AllocFunc(Global_Alloc) {
    if (oldsize == 0 && newsize) {
        Log("Alloc");
        return malloc(newsize);
    }

    if (newsize == 0 && ptr) {
        Log("Free");
        free(ptr);
        return NULL;
    }

    if (oldsize && newsize && ptr) {
        return realloc(ptr, newsize);
    }

    Log("Invalid Alloc Args");
    assert(0);
}

Allocator MakeGlobalAllocator() {
    Allocator a = {
        .alloc = Global_Alloc
    };
    return a;
}
