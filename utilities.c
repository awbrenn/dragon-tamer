#include <stdio.h>
#include <stdlib.h>

void dieWithError(char* message) {
    fprintf(stderr, "%s\n", message);
    exit(-1);
}
