#include "display.h"
#include <stdio.h>


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

void draw(SDL_Renderer *renderer, uint16_t indexReg, uint16_t instr, uint8_t *V, uint8_t *firstAddressMemory,uint64_t *displayArr){

    int N = instr & 0x000F;
    int x = V[instr & 0x0F00] % 64;
    int og_x = x;
    int y = V[instr & 0x00F0] % 32;
    V[0xF] = 0;

    // int spriteWidth = 8; int xOffset = (64 - spriteWidth) / 2; int yOffset = (32 - N) / 2; x = og_x + xOffset; y += yOffset;
    // Above line sets offset.  Examine later

   for(int i = 0; i < N; i++) {
       uint8_t Nth_byte_sprite_data = firstAddressMemory[indexReg + i];

       x = og_x;
       for(int j = 7; j >= 0; j--) {
           bool current_pixel_in_sprite_row = Nth_byte_sprite_data & ((uint8_t)1 << j); //Check bit
           bool pixel_at_xy = displayArr[y] & ((uint64_t)1 << x);
            //Reset x when drawing next row

           if(current_pixel_in_sprite_row) {
               if(pixel_at_xy) {
                   V[0xF] = 1;
                   displayArr[y] &= ~((uint64_t)1 << x); //Turn off bit
               }
               else {
                   displayArr[y] |= ((uint64_t)1 << x); //Turn on bit
               }
           }

           if(x == 63) {
               break;
           }
           x++;
       }

       y++;
       if(y == 32) {
           break;
       }
   }


    //Convert displayArr to pixels on screen
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    for (int j = 0; j < 32; j++) {
        for (int i = 0; i < 64; i++) {
            if ((displayArr[j] >> i) & (uint64_t)1) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect rect = {i * 10, j * 10, 10, 10};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);



}
