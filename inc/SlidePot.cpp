/* SlidePot.cpp
 * Students put your names here
 * Modified: put the date here
 * 12-bit ADC input on ADC1 channel 5, PB18
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/SlidePot.h"
#define ADCVREF_VDDA 0x000
#define ADCVREF_INT  0x200


void SlidePot::Init(void){
// write code to initialize ADC1 channel 5, PB18
// Your measurement will be connected to PB18
// 12-bit mode, 0 to 3.3V, right justified
// software trigger, no averaging

    ADC->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
    ADC->ULLMEM.GPRCM.PWREN = 0x26000001;
    Clock_Delay(24);
    ADC->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
    ADC->ULLMEM.CLKFREQ = 7;
    ADC->ULLMEM.CTL0 = 0x03010000;
    ADC->ULLMEM.CTL1 = 0x00000000;
    ADC->ULLMEM.CTL2 = 0x00000000;
    ADC->ULLMEM.MEMCTL[0] = CHANNEL;
    ADC->ULLMEM.SCOMP0 = 0;
    ADC->ULLMEM.CPU_INT.IMASK = 0;
}

uint32_t SlidePot::In(void){
  uint32_t result = 0;
  uint32_t numSamples = 10;
  for (int i = 0; i < numSamples; i++) {
    ADC->ULLMEM.CTL0 |= 0x00000001;
    ADC->ULLMEM.CTL1 |= 0x00000100;
    while((ADC->ULLMEM.STATUS&0x01) == 0x01) {};
    result += ADC->ULLMEM.MEMRES[0];
  }

  return result / numSamples;
}


// constructor, invoked on creation of class
// m and b are linear calibration coefficents
SlidePot::SlidePot(uint32_t m, uint32_t b){
    slope = m;
    offset = b;
}

void SlidePot::Save(uint32_t n){
  data = n;
  flag = 1;
}
uint32_t SlidePot::Convert(uint32_t n){
    return ((n*slope) >> 12) + offset;
}

// do not use this function
// it is added just to show you how SLOW floating point in on a Cortex M0+
float SlidePot::FloatConvert(uint32_t input){
  return 0.00048828125*input -0.0001812345;
}

void SlidePot::Sync(void){
  while (!flag) {};
  flag = 0;
}


uint32_t SlidePot::Distance(void){
    return distance;
}

