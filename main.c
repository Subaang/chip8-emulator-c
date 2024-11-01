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
        printf("Loaded ROM!");
    }
    else {
        printf("UNSUCCESSFUL LOAD!");
        return(-1);
    }

    //Fonts
    uint8_t fontset[80] =
{
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    int k = 0;
    for(int i = 80; i < 160; i++) { //Store fonts from 0x50 to 0x9F (inclusive)
        firstAddressMemory[i] = fontset[k];
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
