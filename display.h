#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL.h>

extern int processEvents(SDL_Window *window);
extern void doRender(SDL_Renderer *renderer);

#endif
