#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdint.h>

#include "display.h"
#include "memory.h"


int main(int argc, char *argv[]) {
    uint8_t *firstAddressMemory = createMemory();
    uint8_t *PC = firstAddressMemory; //Program counter
    uint16_t *indexReg; //16-bit index register
    int8_t *V[16]; // 16 8-bit registers
    StackElement *firstElementStack = createStack();
    StackElement *stackPointer = firstElementStack;
    uint8_t delayTimer;
    uint8_t soundTimer;




    FILE *rom = fopen("D:/C/Chip8-Emulator-C/IBM Logo.ch8", "rb");

    if(rom != NULL) {
        fseek(rom, 0, SEEK_END);
        int rom_length = ftell(rom);
        rewind(rom);

        fread(firstAddressMemory+512, sizeof(uint8_t), rom_length, rom);
        printf("done!");
    }

    else {
        printf("UNSUCCESSFUL LOAD!");
        return(-1);
    }




    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Game Window",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                            0);

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int done = 0;
    while(!done) {
        done = processEvents(window);
        doRender(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
