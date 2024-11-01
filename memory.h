#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

typedef struct StackElement{
    uint16_t data;
    struct StackElement *next;
}StackElement;

extern uint8_t *createMemory();
extern StackElement *createStack();

#endif
