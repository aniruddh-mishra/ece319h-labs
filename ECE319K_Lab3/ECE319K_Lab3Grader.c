/* ECE319K_Lab3Grader.c
 * Jonathan Valvano
 * December 5, 2023
 * Derived from uart_rw_multibyte_fifo_poll_LP_MSPM0G3507_nortos_ticlang
 */
/*
 Copyright 2023 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"
#include <stdio.h>
#include <string.h>

// PA10 is UART0 Tx    index 20 in IOMUX PINCM table
// PA11 is UART0 Rx    index 21 in IOMUX PINCM table
// Insert jumper J25: Connects PA10 to XDS_UART
// Insert jumper J26: Connects PA11 to XDS_UART
// Switch input PB2 PB1 or PB0
// LED output PB18 PB17 or PB16
// logic analyzer pins PB18 PB17 PB16 PB2 PB1 PB0
// analog scope pin PB20, channel 6
uint32_t Score;


extern const char Lab3EID[20]; // student's EID
extern uint32_t DataBuffer[50];
extern uint32_t TimeBuffer[50];

void Lab3Grader(void){
  uint32_t elapsed,start,end,offset,result,ok;
  uint32_t Period0,Period1,Period2,Period7,Period8;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  UART_Init();
  SysTick->CTRL = 0;           // 1) disable SysTick during setup
  SysTick->LOAD = 0xFFFFFF;    // 2) max
  SysTick->VAL = 0;            // 3) any write to current clears it
  SysTick->CTRL = 0x00000005;  // 4) enable SysTick with core clock
  Score = 0;
  if(strcmp(Lab3EID,"ZZZ123")==0){
    UART_OutString("Please enter your EID in ECE319K_Lab3main.c");
    while(1);
  }
  // grader basically repeats the manual testing
  Score = 0;
  UART_OutString("\n\rLab 3, Spring 2024, Grader. \n\r");
  UART_OutString("EID= "); UART_OutString((char*)Lab3EID); UART_OutString("\n\rStep 1) ");
  Debug_Init(); // calls your Lab 3 function
  Debug_Dump(2);
  Clock_Delay(80000000); // 1s
  Debug_Dump(3);
  uint32_t time = TimeBuffer[0]-TimeBuffer[1]; // should be about 80000000
  uint32_t data0 = DataBuffer[0];
  uint32_t data1 = DataBuffer[1];
  if(data0 == 2) Score += 2;
  if(data1 == 3) Score += 2;
  if((time > 75000000)&&(time < 85000000)) Score += 1;
  UART_OutString("Score= "); UART_OutUDec(Score); UART_OutString("\n\rStep 2) ");
  Debug_Init(); // calls your Lab 3 function
  for(int i = 0; i<2; i++){ // only 16 points
    for(int j=0; j<8; j++){
      Debug_Dump(j);
      Clock_Delay(80000); // 1ms
    }
  }
  ok = 1;
  for(int i=0; i<16;i++){
    if(DataBuffer[i] != (i&0x07)){
      ok = 0;
    }
  }
  if(ok){
    Score += 5;
  }
  UART_OutString("Score= "); UART_OutUDec(Score); UART_OutString("\n\rStep 3) ");
  Debug_Init(); // calls your Lab 3 function
  for(int i = 0; i<10; i++){ // called 80 times to fill and go beyond
    for(int j=0; j<8; j++){
      Debug_Dump(j);
      Clock_Delay(40000); // 0.5ms
    }
  }
  ok = 1;
  for(int i=0; i<50;i++){
    if(DataBuffer[i] != (i&0x07)){
      ok = 0;
    }
  }
  if(ok){
    Period0 = Debug_Period(1); // should be about 80000 (1ms)
    if((Period0 > 75000)&&(Period0 < 85000)) Score += 2;
    Period1 = Debug_Period(2); // should be about 160000 (2ms)
    if((Period1 > 155000)&&(Period1 < 165000)) Score += 2;
    Period2 = Debug_Period(4); // should be about 320000 (4ms)
    if((Period2 > 315000)&&(Period2 < 325000)) Score += 2;
    Period7 = Debug_Period(7); // should be about 320000 (4ms)
    if((Period7 > 315000)&&(Period7 < 325000)) Score += 2;
    Period8 = Debug_Period(8); // should be exactly 0 (no edges)
    if(Period8  == 0) Score += 2;
  }
  UART_OutString("Score= "); UART_OutUDec(Score); UART_OutString("\n\rStep 4) ");
  start = SysTick->VAL;
  end = SysTick->VAL;
  offset = (start-end)&0xFFFFFF; // calibration
  start = SysTick->VAL;
  result = Debug_Dump(1);
  end = SysTick->VAL;
  elapsed = (start-end-offset)&0xFFFFFF; // units of 12.5ns
  if(elapsed < 300){
    Score +=5;
  }
  else{
   UART_OutString("Too slow, execution time= "); UART_OutUDec(elapsed); UART_OutString(" cycles, ");
  }
  UART_OutString("Score= "); UART_OutUDec(Score); UART_OutString(" out of 25\n\rDone\n\r ");

}


