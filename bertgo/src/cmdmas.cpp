#include <cstdio>

void print_bootlogo() {
    const char *hedgehog = 
#include "hedgy"
    ;
    printf("%s", hedgehog);
}