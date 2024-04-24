// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(uint32_t period, uint32_t priority){
// write this
    SysTick->CTRL = 0x00;
    SysTick->LOAD = period - 1;
    SCB->SHP[1] = (SCB->SHP[1] & (~0xC0000000)) | priority << 30;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
//    static uint32_t index = 0;
//    // write this
//    // output one value to DAC
//    DAC5_Out(soundArray[index]);
//    index = (index + 1) % size;
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
// write this
  
}

void Menu_Sound() {

}
