#include <debug.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

static int logfile = STDOUT_FILENO;

void Debug_Print(char* format, ...) {
    va_list args;
    va_start(args, format);
    
    vdprintf(logfile, format, args);

    va_end(args);
}

void SetLogFile(const char* file) {
    if (logfile > 2) close(logfile);
    logfile = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}
