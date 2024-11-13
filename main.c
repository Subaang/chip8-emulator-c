#include <stdio.h>
#include <SDL.h>
#include <stdint.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#include "display.h"
#include "memory.h"

#define FPS 60.0


int randInRange(int min, int max)
{
    return min + (int) (rand() / (double) (RAND_MAX + 1) * (max - min + 1));
}


int main(int argc, char *argv[]) {

    uint8_t *firstAddressMemory = createMemory(); //To access any address, use firstMemoryAddress[PC]
    uint16_t PC = 0x200; //Program counter
    uint16_t indexReg = 0; //16-bit index register
    uint8_t V[16] = {0}; // 16 8-bit registers
    StackElement *stackTop = NULL;
    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;

    uint64_t displayArr[32] = {0}; //Use to hold a monochromatic screen as an array. use bitshifts

    srand(time(NULL));
    FILE *rom = fopen("D:/C/Chip8-Emulator-C/6-keypad.ch8", "rb");

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
    const uint8_t fontset[80] =
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
    for(int i = 0x50; i < 0x50 + 80; i++) {
        firstAddressMemory[i] = fontset[k];
        k++;
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
    while(!done) {

        //TODO: Find a way to limit instructions per second to 700
        //Timers
        if(delayTimer > 0) {
            delayTimer -= 60;
        }

        if(soundTimer > 0) {
            Beep(750,1000.0/FPS);
            soundTimer -= 60;
        }


        uint16_t instr = (firstAddressMemory[PC] << 8) | firstAddressMemory[PC+1];
        PC += 2;

        printf("Executing : %X\n", instr);

        //Decode and Execute cycle
         switch(instr & 0xF000) {
             case 0x0000:
                 switch(instr & 0x0FFF) {
                     //Clear screen
                     case 0x00E0:
                         SDL_RenderClear(renderer);
                     break;

                     case 0x00EE:
                         PC = popStack(&stackTop);

                     break;

                     case 0x0000:
                         printf("End of program\n");
                     break;


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
                 pushStack(&stackTop,PC);
                 PC = instr & 0x0FFF;
             break;

             //Conditional skip
             case 0x3000:
                 if((uint16_t)V[(instr & 0x0F00) >> 8] == (uint16_t)(instr & 0x00FF)) {
                     PC += 2;
                 }
             printf("-------------------\n");
             break;

             //Conditional skip
             case 0x4000:
                 if((uint16_t)V[(instr & 0x0F00) >> 8] != (uint16_t)(instr & 0x00FF)) {
                     PC += 2;
                 }
             break;

             //Conditional skip
             case 0x5000:
                 if(V[(instr & 0x0F00) >> 8] == V[(instr & 0x00F0) >> 4]) {
                     PC += 2;
                 }
             break;

             //Set register VX
             case 0x6000:
                 V[(instr & 0x0F00) >> 8] = instr & 0x00FF;
             break;

             //Add value to reg VX
             case 0x7000:
                 V[(instr & 0x0F00) >> 8] += instr & 0x00FF;
             break;

             //Set
              case 0x8000:
                  switch(instr & 0x000F) {
                      case 0x0000:
                          V[(instr & 0x0F00) >> 8] = V[(instr & 0x00F0) >> 4];
                      break;

                      case 0x0001:
                          V[(instr & 0x0F00) >> 8] |= V[(instr & 0x00F0) >> 4];
                      break;

                      case 0x0002:
                          V[(instr & 0x0F00) >> 8] &= V[(instr & 0x00F0) >> 4];
                      break;

                      case 0x0003:
                          V[(instr & 0x0F00) >> 8] ^= V[(instr & 0x00F0) >> 4];
                      break;

                      case 0x0004:
                          {
                          int sum = V[(instr & 0x0F00) >> 8] + V[(instr & 0x00F0) >> 4];
                          V[(instr & 0x0F00) >> 8] = sum; //unsigned uint8 wraps back to 0 if overflow
                          V[0xF] = sum > 255;
                          break;
                          }

                      case 0x0005:
                      {
                          uint8_t Vx = V[(instr & 0x0F00) >> 8];
                          uint8_t Vy = V[(instr & 0x00F0) >> 4];
                          V[(instr & 0x0F00) >> 8] = Vx - Vy;
                          V[0xF] = Vx >= Vy;
                          break;
                      }

                      case 0x0007:
                      {
                          uint8_t Vx = V[(instr & 0x0F00) >> 8];
                          uint8_t Vy = V[(instr & 0x00F0) >> 4];
                          V[(instr & 0x0F00) >> 8] = Vy - Vx;
                          V[0xF] = Vy >= Vx;
                          break;
                      }

                      case 0x0006: //Modern implementation for ambiguous instruction
                         // V[(instr & 0x0F00) >> 8] = V[(instr & 0x00F0) >> 4];
                         V[0xF] = V[(instr & 0x0F00) >> 8] & 1;
                         V[(instr & 0x0F00) >> 8] >>= 1;
                      break;

                      case 0x000E: //Modern implementation for ambiguous instruction
                          //V[(instr & 0x0F00) >> 8] = V[(instr & 0x00F0) >> 4];
                           V[0xF] = V[(instr & 0x0F00) >> 8] >> 7;
                           V[(instr & 0x0F00) >> 8] <<= 1;
                      break;


                      default:
                          printf("Unknown shift");
                          exit(-1);
                  }
              break;

             //Conditional skip
             case 0x9000:
                 if(V[(instr & 0x0F00) >> 8] != V[(instr & 0x00F0) >> 4]) {
                     PC += 2;
                 }
             break;

             //Set index reg
             case 0xA000:
                 indexReg = instr & 0x0FFF;
             break;

             //Jump with offset
             case 0xB000: //Older implementation for ambiguous instruction
                 PC = instr & 0x0FFF + V[0];
             break;

             //Random
             case 0xC000:
                 V[(instr & 0x0F00) >> 8] = randInRange(0, instr & 0x00FF) & (instr & 0x00FF);
             break;

             //Display DXYN
             case 0xD000:
                 draw(renderer,indexReg,instr,V,firstAddressMemory,displayArr);
                 break;

             //Skip if key
             case 0xE000:
                 switch(instr & 0x00FF) {
                     case 0x009E:
                         if(keyHeldDown(V[(instr & 0x0F00) >> 8])){
                             PC += 2;
                         }
                     break;

                     case 0x00A1:
                         if(!keyHeldDown(V[(instr & 0x0F00) >> 8])){
                             PC += 2;
                         }
                     break;

                     default:
                         printf("Unknown Skip if key");
                         exit(-1);
                 }
             break;

             case 0xF000: // 0xFX__
                 switch(instr & 0x00FF) {
                     //Timers
                     case 0x0007:
                         V[(instr & 0x0F00) >> 8] = delayTimer;
                     break;

                     case 0x0015:
                         delayTimer = V[(instr & 0x0F00) >> 8];
                     break;

                     case 0x0018:
                         soundTimer = V[(instr & 0x0F00) >> 8];
                     break;

                     //Add to index
                     case 0x001E: // Some ambiguous behavior. See guide. Considering overflow here
                     {
                         // if((int)indexReg + (int)V[(instr & 0x0F00) >> 8] > 0xFFF) {
                         //     V[0xF] = 1;
                         // }
                         // else {
                         //     V[0xF] = 0;
                         // }

                         indexReg += V[(instr & 0x0F00) >> 8];

                         break;
                     }


                     //Get key
                     case 0x000A:
                         PC -= 2;
                         if(detectKeyPress()) {
                             V[(instr & 0x0F00) >> 8] = detectKeyPress();
                             PC += 2;
                         }
                     break;

                     //Font Character
                     case 0x0029:
                         indexReg = 0x50 + V[(instr & 0x0F00) >> 8] * 5;
                     break;

                     //Binary-coded decimal conversion
                     case 0x0033:
                         uint8_t VX = V[(instr & 0x0F00) >> 8];
                         //Integer division followed by modulo
                         firstAddressMemory[indexReg] = (VX / 100) % 10;
                         firstAddressMemory[indexReg+1] = (VX / 10) % 10;
                         firstAddressMemory[indexReg + 2] = VX % 10;
                     break;

                     //Store and load memory
                     //Ambiguous instruction - going with modern approach
                     case 0x0055:
                         for(uint16_t i = 0; i <= (instr & 0x0F00) >> 8; i++) {
                             firstAddressMemory[indexReg + i] = V[i];
                         }
                     break;

                     case 0x0065:
                         for(uint16_t i = 0; i <= (instr & 0x0F00) >> 8; i++) {
                             V[i] = firstAddressMemory[indexReg + i];
                         }
                     break;

                     default:
                         printf("Unknown F instruction!");
                         exit(-1);
                 }
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
