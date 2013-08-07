#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <string.h>

#define LEN 100

static void signal_handler(int signum)
{
    void *array[LEN];
    size_t size;
    char **string;
    int i;

    size = backtrace(array, LEN);
    string = (char**) backtrace_symbols(array, size);

    fprintf(stderr, "received SIGSEGV! Stack trace:\n");
    for(i = 0; i<size ; i++) {
        fprintf(stderr, "%d %s\n", i, string[i]);
    }

    free(string);
    exit(1);
}

void ill_func()
{
    int *p = (int*)0x100;
    *p = 100;
}

void another()
{
    ill_func();
}

void some_func()
{
    another();
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, signal_handler);

    some_func();

    return 0;
}
