/* ECE319K_Lab3main.c
 * Debugging Lab3 dump functions
 * December 5, 2023
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
const char Lab3EID[] ="AM97437"; // replace ZZZ123 with your EID here
extern uint32_t DataBuffer[50];
extern uint32_t TimeBuffer[50];
uint32_t Time,Period0,Period1,Period2,Period7,Period8;
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  UART_Init();
  UART_OutString("Lab 3, Spring 2024, Step 1.\n\r");
  UART_OutString("EID= "); UART_OutString((char*)Lab3EID); UART_OutString("\n\r");
  Debug_Init(); // calls your Lab 3 function
  Debug_Dump(2);
  Clock_Delay(80000000); // 1s
  Debug_Dump(3);
  Time = TimeBuffer[0]-TimeBuffer[1]; // should be about 80000000
  UART_OutString("DataBuffer[0]= "); UART_OutUDec(DataBuffer[0]); UART_OutString(", should be 2\n\r");
  UART_OutString("DataBuffer[1]= "); UART_OutUDec(DataBuffer[1]); UART_OutString(", should be 3\n\r");
  UART_OutString("Time= "); UART_OutUDec(Time); UART_OutString(" cycles, should be about 80,000,000\n\r");

  Debug_Dump(4);
  Clock_Delay(8000000); // 0.1s
  Debug_Dump(5);
  Time = TimeBuffer[2]-TimeBuffer[3]; // should be about 8000000
  UART_OutString("DataBuffer[2]= "); UART_OutUDec(DataBuffer[2]); UART_OutString(", should be 4\n\r");
  UART_OutString("DataBuffer[3]= "); UART_OutUDec(DataBuffer[3]); UART_OutString(", should be 5\n\r");
  UART_OutString("Time= "); UART_OutUDec(Time); UART_OutString(" cycles, should be about 8,000,000\n\r");
  // put a breakpoint here and look at your arrays
  while(1);
}
int main2(void){// main2
  Clock_Init80MHz(0);
  LaunchPad_Init();
  UART_Init();
  UART_OutString("Lab 3, Spring 2024, Step 2.\n\r");
  UART_OutString("EID= "); UART_OutString((char*)Lab3EID); UART_OutString("\n\r");
  Debug_Init(); // calls your Lab 3 function
  for(int i = 0; i<2; i++){ // only 16 points
    for(int j=0; j<8; j++){
      Debug_Dump(j);
      Clock_Delay(80000); // 1ms
    }
  }
  for(int i=0; i<16;i++){
    if(DataBuffer[i] != (i&0x07)){
      UART_OutString("Error at index= "); UART_OutUDec(i); UART_OutString("\n\r");
    }
  }
  Period0 = Debug_Period(1); // should be about 160000 (2ms)
  Period1 = Debug_Period(2); // should be about 320000 (4ms)
  Period2 = Debug_Period(4); // should be about 640000 (8ms)
  Period7 = Debug_Period(7); // should be about 640000 (8ms)
  Period8 = Debug_Period(8); // should be exactly 0 (no edges)
  UART_OutString("Period0= "); UART_OutUDec(Period0); UART_OutString(" cycles, should be about 160000 (2ms)\n\r");
  UART_OutString("Period1= "); UART_OutUDec(Period1); UART_OutString(" cycles, should be about 320000 (4ms)\n\r");
  UART_OutString("Period2= "); UART_OutUDec(Period2); UART_OutString(" cycles, should be about 640000 (8ms)\n\r");
  UART_OutString("Period7= "); UART_OutUDec(Period7); UART_OutString(" cycles, should be about 640000 (8ms)\n\r");
  UART_OutString("Period8= "); UART_OutUDec(Period8); UART_OutString(" cycles, should be exactly 0 (no edges)\n\r");
// put a breakpoint here and look at arrays Period0, Period1, Period2,Period7 and Period8
  while(1);
}
int main3(void){// main3
  Clock_Init80MHz(0);
  LaunchPad_Init();
  UART_Init();
  UART_OutString("Lab 3, Spring 2024, Step 3.\n\r");
  UART_OutString("EID= "); UART_OutString((char*)Lab3EID); UART_OutString("\n\r");
  Debug_Init(); // calls your Lab 3 function
  for(int i = 0; i<10; i++){ // called 80 times to fill and go beyond
    for(int j=0; j<8; j++){
      Debug_Dump(j);
      Clock_Delay(40000); // 0.5ms
    }
  }
  for(int i=0; i<50;i++){
    if(DataBuffer[i] != (i&0x07)){
      UART_OutString("Error at index= "); UART_OutUDec(i); UART_OutString("\n\r");
    }
  }
  Period0 = Debug_Period(1); // should be about 80000 (1ms)
  Period1 = Debug_Period(2); // should be about 160000 (2ms)
  Period2 = Debug_Period(4); // should be about 320000 (4ms)
  Period7 = Debug_Period(7); // should be about 320000 (4ms)
  Period8 = Debug_Period(8); // should be exactly 0 (no edges)
  UART_OutString("Period0= "); UART_OutUDec(Period0); UART_OutString(" cycles, should be about 80000 (1ms)\n\r");
  UART_OutString("Period1= "); UART_OutUDec(Period1); UART_OutString(" cycles, should be about 160000 (2ms)\n\r");
  UART_OutString("Period2= "); UART_OutUDec(Period2); UART_OutString(" cycles, should be about 320000 (4ms)\n\r");
  UART_OutString("Period7= "); UART_OutUDec(Period7); UART_OutString(" cycles, should be about 320000 (4ms)\n\r");
  UART_OutString("Period8= "); UART_OutUDec(Period8); UART_OutString(" cycles, should be exactly 0 (no edges)\n\r");
// put a breakpoint here and look at arrays Period0, Period1, Period2,Period7 and Period8
  while(1);
}
int main4(void){// main4
  uint32_t elapsed,start,end,offset,result;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  SysTick->CTRL = 0;           // 1) disable SysTick during setup
  SysTick->LOAD = 0xFFFFFF;    // 2) max
  SysTick->VAL = 0;            // 3) any write to current clears it
  SysTick->CTRL = 0x00000005;  // 4) enable SysTick with core clock
  UART_Init();
  UART_OutString("Lab 3, Spring 2024, Step 4.\n\r");
  UART_OutString("EID= "); UART_OutString((char*)Lab3EID); UART_OutString("\n\r");
  Debug_Init(); // calls your Lab 3 function
  start = SysTick->VAL;
  end = SysTick->VAL;
  offset = (start-end)&0xFFFFFF; // calibration
  start = SysTick->VAL;
  result = Debug_Dump(1);
  end = SysTick->VAL;
  elapsed = (start-end-offset)&0xFFFFFF; // units of 12.5ns
  UART_OutString("Execution time= "); UART_OutUDec(elapsed); UART_OutString(" cycles\n\r");
// this should be less than 300 bus cycles (my solution was 87 cycles)
  while(1);
}
void Lab3Grader(void);
int main(void){ // main5
  Debug_Init(); // calls your Lab 3 function
  Lab3Grader();
  while(1);
}



