#include <stdio.h>
#include <stdlib.h>
#include <util.h>





AllocFunc(GlobalAlloc) {
    if (!newsize) {
        //printf("free\n");
        free(ptr);
        return NULL;
    }

    if (oldsize) {
        //printf("realloc! %d\n", newsize);
        return realloc(ptr, newsize);
    }
    //printf("malloc\n");
    return malloc(newsize);
}


Allocator GlobalAllocator = (Allocator) {
    .a = GlobalAlloc,
};
