/*
 * Sensors.cpp
 *
 *  Created on: Apr 13, 2024
 *      Author: Aniruddh Mishra
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "Sensors.h"
#define ADCVREF_VDDA 0x000
#define ADCVREF_INT  0x200

Sensors::Sensors() {
    // Reset ADC and VREF
    // RSTCLR
    //   bits 31-24 unlock key 0xB1
    //   bit 1 is Clear reset sticky bit
    //   bit 0 is reset ADC
    ADC1->ULLMEM.GPRCM.RSTCTL = (uint32_t)0xB1000003;
    VREF->GPRCM.RSTCTL = (uint32_t)0xB1000003;
    // Enable power ADC and VREF
    // PWREN
    //   bits 31-24 unlock key 0x26
    //   bit 0 is Enable Power
    ADC1->ULLMEM.GPRCM.PWREN = (uint32_t)0x26000001;
    VREF->GPRCM.PWREN = (uint32_t)0x26000001;
    Clock_Delay(24); // time for ADC and VREF to power up
    ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // ULPCLK
    // bits 31-24 key=0xA9
    // bit 5 CCONSTOP= 0 not continuous clock in stop mode
    // bit 4 CCORUN= 0 not continuous clock in run mode
    // bit 1-0 0=ULPCLK,1=SYSOSC,2=HFCLK
    ADC1->ULLMEM.CLKFREQ = 7; // 40 to 48 MHz
    ADC1->ULLMEM.CTL0 = 0x03010000;
    // bits 26-24 = 011 divide by 8
    // bit 16 PWRDN=1 for manual, =0 power down on completion, if no pending trigger
    // bit 0 ENC=0 disable (1 to 0 will end conversion)
    ADC1->ULLMEM.CTL1 = 0x00010000;
    // bits 30-28 =0  no shift
    // bits 26-24 =0  no averaging
    // bit 20 SAMPMODE=0 high phase
    // bits 17-16 CONSEQ=01 ADC at start to end
    // bit 8 SC=0 for stop, =1 to software start
    // bit 0 TRIGSRC=0 software trigger
    ADC1->ULLMEM.CTL2 = 0x01000000;
    // bits 28-24 ENDADD=2 (which  MEMCTL to end)
    // bits 20-16 STARTADD=1 (which  MEMCTL to start)
    // bits 15-11 SAMPCNT (for DMA)
    // bit 10 FIFOEN=0 disable FIFO
    // bit 8  DMAEN=0 disable DMA
    // bits 2-1 RES=0 for 12 bit (=1 for 10bit,=2for 8-bit)
    // bit 0 DF=0 unsigned formant (1 for signed, left aligned)
    ADC1->ULLMEM.MEMCTL[0] = ADCVREF_INT+2; // reference + channel
    ADC1->ULLMEM.MEMCTL[1] = ADCVREF_VDDA+5; // reference + channel
    // bit 28 WINCOMP=0 disable window comparator
    // bit 24 TRIG trigger policy, =0 for auto next, =1 for next requires trigger
    // bit 20 BCSEN=0 disable burn out current
    // bit 16 = AVGEN =0 for no averaging
    // bit 12 = STIME=0 for SCOMP0
    // bits 9-8 VRSEL = 10 for internal VREF,(00 for VDDA)
    // bits 4-0 channel = 0 to 7 available
    ADC1->ULLMEM.SCOMP0 = 0; // 8 sample clocks
    //  ADC1->ULLMEM.GEN_EVENT.ICLR |= 0x0100; // clear flag MEMCTL[1] ??
    ADC1->ULLMEM.GEN_EVENT.IMASK = 0; // no interrupt

    ADC0->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
    ADC0->ULLMEM.GPRCM.PWREN = 0x26000001;
    Clock_Delay(24);
    ADC0->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
    ADC0->ULLMEM.CLKFREQ = 7;
    ADC0->ULLMEM.CTL0 = 0x03010000;
    ADC0->ULLMEM.CTL1 = 0x00000000;
    ADC0->ULLMEM.CTL2 = 0x00000000;
    ADC0->ULLMEM.MEMCTL[0] = 5;
    ADC0->ULLMEM.SCOMP0 = 0;
    ADC0->ULLMEM.CPU_INT.IMASK = 0;

    VREF->CLKSEL = 0x00000008; // bus clock
    VREF->CLKDIV = 0; // divide by 1
    VREF->CTL0 = 0x0001;
    // bit 8 SHMODE = off
    // bit 7 BUFCONFIG=0 for 2.4 (=1 for 1.4)
    // bit 0 is enable
    VREF->CTL2 = 0;
    // bits 31-16 HCYCLE=0
    // bits 15-0 SHCYCLE=0
    while((VREF->CTL1&0x01)==0){}; // wait for VREF to be ready
}

void Sensors::InitializePosition() {
    uint32_t numSamples = 10;
    uint32_t x_sum = 0;
    uint32_t y_sum = 0;
    for (int i = 0; i < numSamples; i++) {
        ADC1->ULLMEM.CTL0 |= 0x00000001; // enable conversions
        ADC1->ULLMEM.CTL1 |= 0x00000100; // start ADC
        while(ADC1->ULLMEM.STATUS&0x01){}; // wait for completion
        if (i > 0) {
            y_sum += ADC1->ULLMEM.MEMRES[0];
        }

        ADC0->ULLMEM.CTL0 |= 0x00000001;
        ADC0->ULLMEM.CTL1 |= 0x00000100;
        while(ADC0->ULLMEM.STATUS&0x01) {};
        if (i > 0) {
            x_sum = ADC0->ULLMEM.MEMRES[0];
        }
    }

    y_base = y_sum/(numSamples - 1);
    x_base = x_sum/(numSamples - 1);
}

void Sensors::In() {
    uint32_t numSamples = 10;
    uint32_t x_sum = 0;
    uint32_t y_sum = 0;
    uint32_t pot_sum = 0;
    for (int i = 0; i < numSamples; i++) {
        ADC1->ULLMEM.CTL0 |= 0x00000001; // enable conversions
        ADC1->ULLMEM.CTL1 |= 0x00000100; // start ADC
        while(ADC1->ULLMEM.STATUS&0x01){}; // wait for completion
        if (i > 0){
            y_sum += ADC1->ULLMEM.MEMRES[0];
            pot_sum += ADC1->ULLMEM.MEMRES[1];
        }

        ADC0->ULLMEM.CTL0 |= 0x00000001;
        ADC0->ULLMEM.CTL1 |= 0x00000100;
        while(ADC0->ULLMEM.STATUS&0x01) {};
        if (i > 0) {
            x_sum = ADC0->ULLMEM.MEMRES[0];
        }
    }

    y_axis = y_sum/(numSamples - 1);
    x_axis = x_sum/(numSamples - 1);
    pot_d = pot_sum/(numSamples - 1);
}

uint32_t Sensors::PotVal() {
    return pot_d;
}

void Sensors::Orientation(int32_t& x_axis, int32_t& y_axis) {
    x_axis = this->x_axis - this->x_base;
    y_axis = this->y_axis - this->y_base;
}
