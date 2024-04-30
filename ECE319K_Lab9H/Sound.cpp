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
#include "../inc/Clock.h"
#include "Sensors.h"

Sensors SlidePot;
bool playOnce;

#define SinWave_Size 32

const uint8_t SinWave[32] = {
     16, 19, 22, 24, 27, 28, 30, 31, 31, 31, 30, 28, 27,
     24, 22, 19, 16, 13, 10, 8, 5, 4, 2, 1, 1, 1, 2, 4,
     5, 8, 10, 13
};

const uint8_t* soundArray = SinWave;
uint32_t size = SinWave_Size;

void SysTick_IntArm(uint32_t period, uint32_t priority){
    SysTick->CTRL = 0x00;
    SysTick->LOAD = period - 1;
    SCB->SHP[1] = (SCB->SHP[1] & (~0xC0000000)) | priority << 30;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
    SysTick_IntArm(1, 0);
  
}


extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
//    static uint32_t index = 0;
//    uint32_t adcin = (4095-SlidePot.PotVal())/512;
//    // write this
//    // output one value to DAC
//    DAC5_Out(soundArray[index]>>adcin);
//    index = (index + 1) % size;
////    if (index == 0) {
////        for (int i = 0; i < 1000; i++) {
////            Clock_Delay1ms(1);
////        }
////    }
    static uint32_t index = 0;
    SlidePot.In();
    uint32_t adcin = (4095-SlidePot.PotVal())/512;
    // write this
    // output one value to DAC
    DAC5_Out(soundArray[index]>>adcin);
    index = (index + 1) % size;
    if (playOnce) {
        if (index == 0) {
            SysTick->LOAD = 0;
            SysTick->VAL = 0;
        }
    }

}

void Sound_Start(uint32_t period, bool once){
    // write this
    // set reload value
    // write any value to VAL, cause reload
    SysTick->LOAD = period - 1;
    SysTick->VAL = 0;
    playOnce = once;
}
void Sound_Shoot(void){
// write this
  
}
void Sound_Killed(void){
// write this
  
}
void Sound_Explosion(void){
// write this
  
}

void Sound_Fastinvader1(void){
  
}
void Sound_Fastinvader2(void){
  
}
void Sound_Fastinvader3(void){
  
}
void Sound_Fastinvader4(void){
  
}
void Sound_Highpitch(void){
  
}
