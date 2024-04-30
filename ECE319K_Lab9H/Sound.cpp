// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "Sensors.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "../inc/Clock.h"

extern Sensors inputs;
int8_t backgroundTrack = -1;

const uint8_t* soundArray;
uint32_t size;

const uint8_t* buff[8];
uint32_t size_buff[8];
uint32_t numQue;

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
    static uint32_t index = 0;
    uint32_t pot_d = inputs.PotVal();
    uint32_t shifter = (4095-pot_d)/512;
    DAC5_Out(soundArray[index] >> shifter);
    index = (index + 1) % size;
    if (index == 0) {
        if (soundArray == buff[0] && numQue > 0) {
            numQue -= 1;
            for (int i = 0; i < numQue; i++) {
                buff[i] = buff[i + 1];
                size_buff[i] = size_buff[i + 1];
            }
        }
        Background_Song_Set(backgroundTrack);
    }
}

void Sound_Start(uint32_t period){
    SysTick->LOAD = period - 1;
    SysTick->VAL = 0;
}

void Sound_Stop() {
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
}

void Background_Song_Set(int8_t track) {
    if (numQue > 0) {
        soundArray = buff[0];
        size = size_buff[0];
    }
    else {
        backgroundTrack = track;
        switch(track) {
            case 0:
                soundArray = gameMusic;
                size = gameMusic_Size;
                break;
            // case 1:
            //     soundArray = menuLoop;
            //     size = menuLoop_Size;
            //     break;
            default:
                Sound_Stop();
                break;
        }
    }
    if (!SysTick->LOAD) Sound_Start(7111);
}

void Sound_Shoot(void){
    if (soundArray != gameMusic) {
        if (numQue >= 8) return;
        buff[numQue] = shoot;
        size_buff[numQue] = shoot_Size;
        numQue ++;
    }
    else {
        soundArray = shoot;
        size = shoot_Size;
        Sound_Start(7111);
    }
}

void Sound_Empty(void){
    if (soundArray != gameMusic) {
        if (numQue >= 8) return;
        buff[numQue] = empty;
        size_buff[numQue] = empty_Size;
        numQue ++;
    }
    soundArray = empty;
    size = empty_Size;
    Sound_Start(7111);
}

void Sound_EnemyHit(void){
    soundArray = enemyHit;
    size = enemyHit_Size;
    Sound_Start(7111);
}

void Sound_Explosion(void){
    soundArray = explosion;
    size = explosion_Size;
    Sound_Start(7111);
}