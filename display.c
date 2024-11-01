#include "display.h"


int processEvents(SDL_Window *window) {
    SDL_Event event;
    int done = 0;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT_CLOSE:
                if(window) {
                    SDL_DestroyWindow(window);
                    window = NULL;
                    done = 1;
                }
            break;

            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        done = 1;
                    break;
                }

            break;
            case SDL_QUIT:
                done = 1;
        }
    }
    return done;
}

void doRender(SDL_Renderer *renderer) { //pass by referrence
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

    SDL_RenderPresent(renderer);

}
