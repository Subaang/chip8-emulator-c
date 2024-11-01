#include "memory.h"

#include <stdlib.h>

uint8_t *createMemory() {
    uint8_t *firstAddress = calloc(1024,sizeof(uint8_t)); //4 kilo bytes in total. each int is 4 bytes
    return firstAddress;
}

StackElement *createStack() {
    StackElement *res = malloc(sizeof(StackElement));
    return res;
}