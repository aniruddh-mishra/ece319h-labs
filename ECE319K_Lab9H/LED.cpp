/*
 * LED.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author: Aniruddh Mishra
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    IOMUX->SECCFG.PINCM[PA24INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PA25INDEX] = 0x81;
    IOMUX->SECCFG.PINCM[PA26INDEX] = 0x81;

    for (int i = 24; i < 27; i++) {
        GPIOA->DOESET31_0 = 1 << i;
    }
}

void LED_Array(uint32_t data) {
    GPIOA->DOUT31_0 = GPIOA->DOUT31_0 & ~(7 << 24) | data << 24;
}

// data specifies which LED to turn on
void LED_On(uint32_t data){
    // use DOUTSET31_0 register so it does not interfere with other GPIO
    // Red LED: 0
    // Yellow LED: 1
    // Green LED: 2
    GPIOA->DOUTSET31_0 = 1 << (24 + data);
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    // use DOUTCLR31_0 register so it does not interfere with other GPIO
    // Red LED: 0
    // Yellow LED: 1
    // Green LED: 2
    GPIOA->DOUTCLR31_0 = 1 << (24 + data);
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    // use DOUTTGL31_0 register so it does not interfere with other GPIO
    // Red LED: 0
    // Yellow LED: 1
    // Green LED: 2
    GPIOA->DOUTTGL31_0 = 1 << (24 + data);
}
