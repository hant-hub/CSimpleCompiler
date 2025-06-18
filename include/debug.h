#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>


#define Log(x, ...) Debug_Print("["__FILE__ ":%d]: " x "\n", __LINE__,##__VA_ARGS__) 

#define assert(x) \
    do { \
        if (x) { \
            break; \
        } \
        Log("Assertion Failure: %s", #x);\
        exit(139); \
    } while(0);

void Debug_Print(char* format, ...);
void SetLogFile(const char* file);



#endif
