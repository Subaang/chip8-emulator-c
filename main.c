#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdint.h>
#include <windows.h>
#include <math.h>

#include "display.h"
#include "memory.h"

#define FPS 60.0


int main(int argc, char *argv[]) {
    uint8_t *firstAddressMemory = createMemory(); //To access any address, use firstMemoryAddress[PC]
    uint16_t PC = 0; //Program counter
    uint16_t indexReg; //16-bit index register
    int8_t V[16]; // 16 8-bit registers
    StackElement *stackTop = NULL;
    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;

    uint64_t displayArr[32] = {0}; //Use to hold a monochromatic screen as an array. use bitshifts


    FILE *rom = fopen("D:/C/Chip8-Emulator-C/IBM Logo.ch8", "rb");

    if(rom != NULL) {
        fseek(rom, 0, SEEK_END);
        int rom_length = ftell(rom);
        rewind(rom);

        fread(firstAddressMemory+512, sizeof(uint8_t), rom_length, rom);
        printf("Loaded ROM!\n");
    }
    else {
        printf("UNSUCCESSFUL LOAD!");
        return(EXIT_FAILURE);
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

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    int done = 0;
    int j = 0;
    while(!done) {
        //Timers
        if(delayTimer > 0) {
            delayTimer -= 60;
        }

        if(soundTimer > 0) {
            Beep(750,1000.0/FPS);
            soundTimer -= 60;
        }


        uint16_t instr = (firstAddressMemory[512+PC] << 8) | firstAddressMemory[512+PC+1];
        PC += 2;


        //Decode and Execute cycle
        switch(instr & 0xF000) {
            case 0x0000:
                switch(instr & 0x0FFF) {
                    //Clear screen
                    case 0x00E0:
                        SDL_RenderClear(renderer);
                    break;

                    case 0x00EE:
                        PC = popStack(stackTop);
                    break;

                    case 0x0000:
                        printf("End of program");
                        SDL_Delay(10000);
                        exit(0);

                    default:
                        printf("Skipping 0NNN - %X\n",instr);

                }
            break;

            //Jump
            case 0x1000:
                PC = instr & 0x0FFF;
            break;

            //Call subroutine at NNN
            case 0x2000:
                pushStack(stackTop,PC);
                PC = instr & 0x0FFF;
            break;

            //Conditional skip
            case 0x3000:
                if(V[instr & 0x0F00] == instr & 0x00FF) {
                    PC += 2;
                }
            break;

            //Conditional skip
            case 0x4000:
                if(V[instr & 0x0F00] != instr & 0x00FF) {
                    PC += 2;
                }
            break;

            //Conditional skip
            case 0x5000:
                if(V[instr & 0x0F00] == V[instr & 0x00F0]) {
                    PC += 2;
                }
            break;

            //Conditional skip
            case 0x9000:
                if(V[instr & 0x0F00] != V[instr & 0x00F0]) {
                    PC += 2;
                }
            break;

            //Set register VX
            case 0x6000:
                V[instr & 0x0F00] = instr & 0x00FF;
            break;

            //Add value to reg VX
            case 0x7000:
                V[instr & 0x0F00] += instr & 0x00FF;
            break;

            //Set
            case 0x8000:
                switch(instr & 0x000F) {
                    case 0x0000:
                        V[instr & 0x0F00] = V[instr & 0x00F0];
                    break;

                    case 0x0001:
                        V[instr & 0x0F00] |= V[instr & 0x00F0];
                    break;

                    case 0x0002:
                        V[instr & 0x0F00] &= V[instr & 0x00F0];
                    break;

                    case 0x0003:
                        V[instr & 0x0F00] ^= V[instr & 0x00F0];
                    break;

                    case 0x0004:
                        int temp = V[instr & 0x0F00] + V[instr & 0x00F0];
                        V[instr & 0x0F00] += V[instr & 0x00F0]; //unsigned int8 wraps back to 0 if overflow

                        if(temp > 255) {
                            V[0xF] = 1;
                        }
                        else {
                            V[0xF] = 0;
                        }
                    break;

                    case 0x0005:
                        if(V[instr & 0x0F00] > V[instr & 0x00F0]) {
                            V[0xF] = 1;
                        }
                        else {
                            V[0xF] = 0;
                        }

                        V[instr & 0x0F00] = V[instr & 0x0F00] - V[instr & 0x00F0];

                    break;

                    case 0x0007:
                        if(V[instr & 0x0F00] > V[instr & 0x00F0]) {
                            V[0xF] = 1;
                        }
                        else {
                            V[0xF] = 0;
                        }

                    V[instr & 0x0F00] = V[instr & 0x00F0] - V[instr & 0x0F00];

                    case 0x0006: //Modern implementation for ambiguous instruction
                        V[0xF] = V[instr & 0x0F00] & 1;
                        V[instr & 0x0F00] = V[instr & 0x0F00] >> 1;
                    break;

                    case 0x000E:
                        V[0xF] = V[instr & 0x0F00] & (1 << 4);
                        V[instr & 0x0F00] = V[instr & 0x0F00] << 1;
                    break;

                    default:
                        printf("Unknown shift");
                        exit(-1);
                }




            //Set index reg
            case 0xA000:
                indexReg = instr & 0x0FFF;
            break;

            //Display DXYN
            case 0xD000:
                draw(renderer,indexReg,instr,V,firstAddressMemory,displayArr);
                break;




            default:
                printf("Unknown opcode %X \n",instr);
                SDL_Delay(10000);
                exit(EXIT_FAILURE);
        }


        done = processEvents(window);
        SDL_Delay(1000.0/FPS);
    }



    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
