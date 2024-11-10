#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

typedef struct StackElement{
    uint16_t data;
    struct StackElement *next;
}StackElement;

extern uint8_t *createMemory();
extern void pushStack(StackElement *top, uint16_t data);
extern uint16_t popStack(StackElement *top);

#endif
