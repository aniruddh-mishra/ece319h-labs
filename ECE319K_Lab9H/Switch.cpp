/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author: Aniruddh Mishra
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "Switch.h"

static uint32_t switchVal;

// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    IOMUX->SECCFG.PINCM[PA15INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PA16INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081;
    IOMUX->SECCFG.PINCM[PA28INDEX] = 0x00040081;
}

// return current state of switches
void Switch_In(void){
  switchVal = GPIOA->DIN31_0 >> 15 & 0x3;
  switchVal += GPIOA->DIN31_0 >> 25 & 0xC;
}

// returns whether input switch is clicked
bool Switch_Active(uint8_t button) {
    return switchVal & button;
}

uint32_t Switch_FSM_Val() {
    uint32_t any = (switchVal != 0);
    return any | Switch_Active(2) << 1;
}
