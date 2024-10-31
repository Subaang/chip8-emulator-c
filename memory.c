#include "memory.h"

#include <stdlib.h>

void *createMemory() {
    void *firstAddress = malloc(4096); //4 kilo bytes in total. each int is 4 bytes
    return firstAddress;
}

StackElement *createStack() {
    StackElement *res = malloc(sizeof(StackElement));
    return res;
}