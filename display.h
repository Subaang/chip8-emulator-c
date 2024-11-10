#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL.h>

extern int processEvents(SDL_Window *window);
extern void draw(SDL_Renderer *renderer, uint16_t indexReg, uint16_t instr, uint8_t *V, uint8_t *firstAddressMemory,uint64_t *displayArr);

#endif
