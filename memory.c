#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

void pushStack(StackElement **top, uint16_t data) {
    StackElement *newElement = malloc(sizeof(StackElement));
    if(newElement == NULL) {
        printf("Stack Overflow");
        exit(1);
    }

    newElement->data = data;
    newElement->next = *top;
    *top = newElement;
}

uint16_t popStack(StackElement **top) {
    if(top == NULL) {
        printf("Stack underflow");
        exit(1);
    }

    uint16_t res = (*top)->data;
    StackElement *temp = *top;
    *top = (*top)->next;
    free(temp);
    temp = NULL;

    return res;
}


uint8_t *createMemory() {
    uint8_t *firstAddress = calloc(4096,sizeof(uint8_t)); //calloc initializes with zeros
    return firstAddress;
}
