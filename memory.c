#include "memory.h"

#include <stdlib.h>

uint8_t *createMemory() {
    uint8_t *firstAddress = calloc(4096,sizeof(uint8_t)); //calloc initializes with zeros
    return firstAddress;
}

StackElement *createStack() {
    StackElement *res = malloc(sizeof(StackElement));
    return res;
}