// Lab7HMain.c
// Runs on MSPM0G3507
// Lab 7 
// Aniruddh Mishra, Evan Lai
// Last Modified: 12/29/2023

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);

// in ECE319H Lab 7, all classes will be statically allocated
SlidePot Sensor(1645,87); // needs calibration

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

// implement this function
void OutFix(uint32_t n){
// resolution is 0.001cm
// n is integer 0 to 2000
// output to ST7735 0.000cm to 2.000cm
  printf("d=%d.%.3d cm ", n/1000, n%1000);
  // write this
}

// do not use this function
// it is added just to show you how SLOW floating point in on a Cortex M0+
void FloatOutFix(float x){
// resolution cm
// c is integer 0 to 2.000
// output to ST7735 0.000cm to 2.000cm
  printf("d=%f cm   ",x);  // floating point output
}

uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
float FloatPosition;  // 32-bit floating-point cm
uint32_t startTime,stopTime;
uint32_t Offset,ADCtime,Converttime,FloatConverttime,OutFixtime,FloatOutFixtime; // in bus cycles
uint32_t Time;
// use main1 if you do not have a voltmeter
// use main1 to test slidepot interface
// connect slidepot pin 2 to channel 5, PB18
// Open TExaSdisplay and see slidepot pin2 go from 0 to 3.3V
int main1(void){ // main1
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  TExaS_Init(ADC,CHANNEL,0); // channel 5, PB18, slidepot
  __enable_irq();
  while(1){
  }
}

// use main2 if you have a voltmeter/scope,
// use the voltmeter/scope see slidepot pin2 go from 0 to 3.3V
// test ADCinit and ADCin functions
// connect slidepot pin 2 to channel 5, PB18
// Data should go from 0 to 4095
// ADCtime is the time to execute Sensor.In in bus cycles
// use main2 to calibrate the system fill in 5 points in Calibration.xls
//    determine constants k1 k2 to fit Position=(k1*Data)>>12 + k2
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  SysTick->LOAD = 0xFFFFFF;    // max
  SysTick->VAL = 0;            // any write to current clears it
  SysTick->CTRL = 0x00000005;  // enable SysTick with core clock
  startTime = SysTick->VAL;
  stopTime = SysTick->VAL;
  Offset = (startTime-stopTime)&0x0FFFFFF; // in bus cycles
  Sensor.Init(); //PB18 = ADC1 channel 5, slidepot
  while(1){
    startTime = SysTick->VAL;
    Data = Sensor.In();  // sample 12-bit ADC0 channel 5, slidepot
    stopTime = SysTick->VAL;
    ADCtime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
  }
}

// use main3 to test Convert functions
// connect slidepot pin 2 to PB18
// Data should go from 0 to 4095
// Position should go from 0 to 2000
// Use main3 to take another 5 measurements to determine accuracy
// truth is you eyes and the cursor on your slide pot
// measurement is the Position variable
// Converttime is the time to execute Convert in bus cycles
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  SysTick->LOAD = 0xFFFFFF;    // max
  SysTick->VAL = 0;            // any write to current clears it
  SysTick->CTRL = 0x00000005;  // enable SysTick with core clock
  startTime = SysTick->VAL;
  stopTime = SysTick->VAL;
  Offset = (startTime-stopTime)&0x0FFFFFF; // 25nsec
  Sensor.Init(); //ADC1 channel 5, PB18, slidepot
  while(1){
    Data = Sensor.In();  // sample 12-bit ADC1 channel 5, PB18, slidepot
    startTime = SysTick->VAL;
    Position = Sensor.Convert(Data);
    stopTime = SysTick->VAL;
    Converttime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
    startTime = SysTick->VAL;
    FloatPosition = Sensor.FloatConvert(Data);
    stopTime = SysTick->VAL;
    FloatConverttime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
  }
}


// use main4 to test OutFix functions
// connect slidepot pin 2 to ADC1 channel 5, PB18
// Data should go from 0 to 4095
// Position should go from 0 to 2000
// LCD should show 0.000cm to 2.000 cm
// OutFixtime is the time to execute OutFix in bus cycles
int main4(void){ // main4
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  //note: if you colors are weird, see different options for
  // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  SysTick->LOAD = 0xFFFFFF;    // max
  SysTick->VAL = 0;            // any write to current clears it
  SysTick->CTRL = 0x00000005;  // enable SysTick with core clock
  Sensor.Init(); //ADC1 channel 5, PB18, slidepot
  while(1){
    ST7735_SetCursor(0,0); // top left
    Data = Sensor.In();  // sample 12-bit ADC1 channel 5, PB18, slidepot
    Position = Sensor.Convert(Data);
    FloatPosition = Sensor.FloatConvert(Data);
    startTime = SysTick->VAL;
    OutFix(Position);
    stopTime = SysTick->VAL;
    OutFixtime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
    ST7735_SetCursor(0,1); // second row
    startTime = SysTick->VAL;
    FloatOutFix(FloatPosition);
    stopTime = SysTick->VAL;
    FloatOutFixtime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
    Clock_Delay1ms(100);
  }
}
// sampling frequency is 30 Hz
void TIMG12_IRQHandler(void){
	// complete this
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    Time++;
    // sample 12-bit ADC1 channel 5, PB18, slidepot
    // store data into mailbox
    // set the semaphore
    Data = Sensor.In();
    Sensor.Save(Data);
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}
// use main5 for final system
// use scope or logic analyzer to verify real time samples
// option 1) remove call to TExaS_Init and use a real scope on PB27
// option 2) use TExaS logic analyzer
int main(void){ // main5
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); //PB18 = ADC1 channel 5, slidepot
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
  ST7735_PlotClear(0,2000);

  TimerG12_IntArm(0x28B0AA, 2);
//  TimerG12_IntArm(39999825, 2); // 1 Hz

  startTime = SysTick->VAL;
  stopTime = SysTick->VAL;
  Offset = (startTime-stopTime)&0x0FFFFFF; // 25nsec
  Time = 0;
  __enable_irq();

  while(1){
	  // complete this
    // wait for semaphore using a call to a method in Sensor
    Sensor.Sync();
    GPIOB->DOUTTGL31_0 = RED; // toggle PB26 (minimally intrusive debugging)
     // toggle red LED2 on Port B, PB26
    Position = Sensor.Convert(Data);
      // convert Data to Position
    // move cursor to top
    ST7735_SetCursor(0,0); // top left
    // display distance in top row OutFix
    OutFix(Position);
    Time++;
    if((Time%15)==0){
      ST7735_PlotPoint(Position);
      ST7735_PlotNextErase(); // data plotted at about 2 Hz
    }
  }
}

// use main6 to study Central Limit Theorem
uint32_t Histogram[64]; // probability mass function
uint32_t Center,SAC;
#define SCALE 2000
int main6(void){ // main6
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); //PB18 = ADC1 channel 5, slidepot
  SAC = 1;
  while(1){int i; uint32_t d,sum;
    sum = 0;
    for(int j=0; j<100; j++){
      sum += Sensor.In();  // sample 12-bit ADC1 channel 5, slidepot
    }
    Center = sum/100; // averaging
    for(i=0; i<64; i++) Histogram[i] = 0; // clear
    for(i=0; i<200; i++){
      sum = 0;
      for(int j=0; j<SAC; j++){
        sum += Sensor.In();  // sample 12-bit ADC0 channel 5, slidepot
      }
      Data = sum/SAC; // averaging
      if(Data<Center-32){
         Histogram[0]++;
      }else if(Data>=Center+32){
         Histogram[63]++;
      }else{
        d = Data-Center+32;
        Histogram[d]++;
      }
    }
    ST7735_PlotClear(0,100);
    ST7735_SetCursor(0,0);
    printf("SAC=%3u,Center=%4u",SAC,Center);
    for(i=0; i<63; i++){
      if(Histogram[i]>99) Histogram[i]=99;
      ST7735_PlotBar(Histogram[i]);
      ST7735_PlotNext();
      ST7735_PlotBar(Histogram[i]);
      ST7735_PlotNext();
    }
    while(LaunchPad_InS2()==0x00040000){}; // wait for release
    while(LaunchPad_InS2()==0){};          // wait for touch
    SAC = SAC<<1;
    if(SAC >= 256) SAC = 1;
  }
}

