// Lab8Main.c
// Runs on MSPM0G3507
// Lab 8
// Aniruddh Mishra
// Last Modified: 4/8/2024
// PA8 GPIO output to IR LED, to other microcontroller IR sensor, PA22 UART2 Rx

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/FIFO2.h"
#include "IRxmt.h"
#include "UART2.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
// ****note to students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}
uint32_t ReceiveCount,TransmitCount;
uint32_t Position; // fixed point 0.001cm, in receiver
uint32_t Data;     // 12-bit raw ADC data in transmitter

SlidePot Sensor(1645,87); // copy calibration from Lab 7

// Implement the FIFO class
//   FIFO2.h is prototype
//   FIFO2.cpp is implementation
// run main1R program on receiver to test FIFO2
// use real scope on PB27 to measure execution time of Put
// use real scope on PB26 to measure execution time of Get
// Each FIFO.print should be 10 sequential characters
// err should be 0
Queue FIFO;
int err,count;
int main1R(void){ // main1R
  char data = 0; char out;
  bool result;
  __disable_irq();
  PLL_Init();     // set system clock to 80 MHz
  LaunchPad_Init();
  ST7735_InitPrintf();
  while(1){
    count=0;
    for(int i=0; i<10; i++){
      GPIOB->DOUTSET31_0 = GREEN;
      result = FIFO.Put(data);
      GPIOB->DOUTCLR31_0 = GREEN;
      if(result){
        count++;
        data++;
      }else{
        err++;
      }
    }
    FIFO.Print();
    for(int i=0; i<count; i++){
      GPIOB->DOUTSET31_0 = RED;
      result = FIFO.Get(&out);
      GPIOB->DOUTCLR31_0 = RED;
      if(!result){
        err++;
      }
    }
    Clock_Delay1ms(1000);
  }
}
// Implement IRxmt_Init and IRxmt_SendBit
//   IRxmt.h is prototype
//   IRxmt.cpp is implementation
// run main1T on transmitter to test IRxmt_SendBit(0);
// Connect scope to PA8 on transmitter
//   verify continuous wave is 38 kHz
//   low time is about 13.158us
//   high time is about 13.158us
int main1T(void){ // main1T
  __disable_irq();
  PLL_Init();     // set system clock to 80 MHz
  LaunchPad_Init();
  IRxmt_Init();    // initialize transmitter, baud rate is 2375 bits/sec
  while(1){
    IRxmt_SendBit(0); // continuous 38 kHz squarewave
  }
}

// Implement IRxmt_Init and IRxmt_SendBit
//   IRxmt.h is prototype
//   IRxmt.cpp is implementation
// run main2T on transmitter to test low-level IR transmission
// Connect scope to PA8 on transmitter
//   verify wave is 38 kHz 
//   verify there are 16 pulses when sending 0
//   verify each bit time is 421.05us, both 0 and 1
int main2T(void){ // main2T
  __disable_irq();
  PLL_Init();     // set system clock to 80 MHz
  LaunchPad_Init();
  IRxmt_Init();    // initialize transmitter, baud rate is 2375 bits/sec
  while(1){
    IRxmt_SendBit(1);
    IRxmt_SendBit(0);
  }
}

// Implement IRtranmission_OutChar
// Implement UART2_Init and UART2_InChar
// interface IR LED using ULN2003B on transmitter
// interface IR sensor on receiver
// Connect dual channel scope to transmitter sensor PA8, sensor output, receiver PA22
//   run main3T on transmitter
//   run main3R on receiver
//   verify pulses on transmitter convert to UART protocol on sensor output, receiver PA22
//   verify waveforms similar to Figure 8.9
int main3T(void){ // main3T
  __disable_irq();
  PLL_Init();     // set system clock to 80 MHz
  LaunchPad_Init();
  IRxmt_Init();    // initialize transmitter, baud rate is 2375 bits/sec
  while(1){ // sends any character over and over
    IRxmt_OutChar(0x29);
    Clock_Delay1ms(500); // about 2Hz
  }
}
char data1,data2,data3,data4;
int main3R(void){ // main3R
  __disable_irq();
  PLL_Init();   // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  UART2_Init(); // just receive, PA22, receiver timeout interrupt synchronization
  __enable_irq();       // interrupts for UART2
  while(1){ // receive a single character
    data1 = UART2_InChar(); // should make main3T
    ST7735_OutChar(data1);
    ST7735_OutChar('\n');
  }
}


// Connect dual channel scope to transmitter sensor PA8, sensor output, receiver PA22
//   run main4T on transmitter
//   run main4R on receiver, start receiver first
//   verify pulses on transmitter convert to UART protocol on sensor output, receiver PA22
int main4T(void){ // main4T
  __disable_irq();
  PLL_Init();     // set system clock to 80 MHz
  LaunchPad_Init();
  IRxmt_Init();    // initialize transmitter, baud rate is 2375 bits/sec
  while(1){ // message is 1.234 cm
    IRxmt_OutChar(0x80|'1'); // 0xB1
    IRxmt_OutChar('2'); // 0x32
    IRxmt_OutChar('3'); // 0x33
    IRxmt_OutChar('4'); // 0x34
    Clock_Delay1ms(500); // about 2Hz
  }
}
int main4R(void){ // main4R
  __disable_irq();
  PLL_Init();     // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  UART2_Init();   // just receive, PA22, receiver timeout interrupt synchronization
  __enable_irq(); // interrupts for UART2
  while(1){ // message is 1.234 cm
    data1 = UART2_InChar(); // should be B1
    ST7735_OutChar(data1&0x7F);
    ST7735_OutChar('.');    // implied
    data2 = UART2_InChar(); // should be 32
    ST7735_OutChar(data2);
    data3 = UART2_InChar(); // should be 33
    ST7735_OutChar(data3);
    data4 = UART2_InChar(); // should be 34
    ST7735_OutChar(data4);
    ST7735_OutChar('\n');
  }
}


// sampling frequency is 30 Hz
// should take 4*10*421.06us = 16.84ms
void TIMG12_IRQHandler(void){uint32_t pos;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    // increment TransmitCount
    TransmitCount++;
    // sample 12-bit ADC0 channel 5, slidepot
    Data = Sensor.In();
    Sensor.Save(Data);
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    uint32_t Position = Sensor.Convert(Data);
    // convert to fixed point distance
    // output 4-frame message
    uint32_t divisor = 1000;

    for (int i = 0; i < 4; i++) {
        char digit = ((Position / divisor) % 10) + 0x30;
        if (i == 0) {
            digit |= 0x80;
        }
        divisor /= 10;
        IRxmt_OutChar(digit);
    }

    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
// Connect dual channel scope to transmitter sensor PA8, sensor output, receiver PA22
//   run main5T on transmitter
//   run main5R on receiver, should work regardless of which is started first
//   verify pulses on transmitter convert to UART protocol on sensor output, receiver PA22
// Data should go from 0 to 4095 in transmitter
// Position should go from 0 to 2000 in receiver
// LCD should show 0.000cm to 2.000 cm
int main5T(void){ // main5T
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  TransmitCount=0;
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  IRxmt_Init();  // just transmit, PA8, blind synchronization
      // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(0x28B0AA, 2);
  __enable_irq();
  while(1){
	  // nothing to do here
  }
}

int main(void){ // main5R
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ReceiveCount=0;
  UART2_Init(); // just receive, PA22, receiver timeout synchronization
  ST7735_InitPrintf();
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_PlotClear(0,2000);
  __enable_irq();
  while(1){
    // move cursor to top left
    ST7735_SetCursor(0,0); // top left
    // wait for first frame
    // incremenet ReceiveCount
    ReceiveCount ++;
    GPIOB->DOUTTGL31_0 = RED; // toggle PB26 (minimally intrusive debugging)

    Position = 0;

    char digit = 0;

    while (!(digit & 0x80)) {
        digit = UART2_InChar();
    }

    ST7735_OutChar(digit&0x7F);
    Position = ((digit&0x7F) - 0x30) * 1000;
    ST7735_OutChar('.');

    uint32_t multiplier = 100;

    for (int i = 0; i < 3; i++) {
        char digit = UART2_InChar();
        Position += (digit - 0x30) * multiplier;
        multiplier /= 10;
        ST7735_OutChar(digit);
    }

    printf(" cm\n");

    // receive next three bytes of message
    // output message

    // calculate Position from the message
    if((ReceiveCount%5)==0){
      ST7735_PlotPoint(Position);
      ST7735_PlotNextErase(); // data plotted at about 2 Hz
    }
  }
}




