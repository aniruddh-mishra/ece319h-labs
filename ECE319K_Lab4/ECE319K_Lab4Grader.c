/* ECE319K_Lab4Grader.c
 * Jonathan Valvano
 * August 28, 2023
 * Derived from uart_rw_multibyte_fifo_poll_LP_MSPM0G3507_nortos_ticlang
 */
/*
 Copyright 2024 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

extern const char EID1[20];
extern const char EID2[20];
static int WestMode=0;    // randomly assigns 0 to 1
static int SouthMode=0;   // randomly assigns 0 to 2
static int SensorMode=0;  // randomly assigns 0 to 3
static int PatternMode=0; // randomly assigns 0 to 1
static int Fail=0;
uint32_t Time=0;
static uint32_t Score=0;

void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
uint32_t M;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t M6;
uint32_t Random6(void){
  M6 = 1664525*M6+1013904223;
  return M6>>26; // 0 to 63
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}
char setUpper(char in){
  if((in >= 'a')&&(in <= 'z')){
    return in-0x20;
  }
  return in;
}
void CheckInitialization(void){
  uint32_t m0,m1,m2,doe;
  UART_OutString("Initialization, ");
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
    Fail = 1;
    return;
  }
  switch(SouthMode){
    case 0:
      m0 = IOMUX->SECCFG.PINCM[PB0INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB1INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB2INDEX];
      doe = (GPIOB->DOE31_0&0x07) != 0x07;
      break;
    case 1:
      m0 = IOMUX->SECCFG.PINCM[PB1INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB2INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB3INDEX];
      doe = (GPIOB->DOE31_0&0x0E) != 0x0E;
      break;
    case 2:
      m0 = IOMUX->SECCFG.PINCM[PB2INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB3INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB4INDEX];
      doe = (GPIOB->DOE31_0&0x1C) != 0x1C;
      break;
  }
  if((m0 != 0x00000081)||(m1 != 0x00000081)||(m2 != 0x00000081)){
    UART_OutString("PINCM register for the South pins should be 0x00000081.\n\r");
    Fail = 1;
    return;
  }
  if(doe){
    UART_OutString("GPIOB->DOE31_0 register for the South pins should be set.\n\r");
    Fail = 1;
    return;
  }
  switch(WestMode){
    case 0:
      m0 = IOMUX->SECCFG.PINCM[PB6INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB7INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB8INDEX];
      doe = (GPIOB->DOE31_0&0x1C0) != 0x1C0;
      break;
    case 1:
      m0 = IOMUX->SECCFG.PINCM[PB7INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB8INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB9INDEX];
      doe = (GPIOB->DOE31_0&0x380) != 0x380;
      break;
  }
  if((m0 != 0x00000081)||(m1 != 0x00000081)||(m2 != 0x00000081)){
    UART_OutString("PINCM register for the West pins should be 0x00000081.\n\r");
    Fail = 1;
    return;
  }
  if(doe){
    UART_OutString("GPIOB->DOE31_0 register for the West pins should be set.\n\r");
    Fail = 1;
    return;
  }
  switch(SensorMode){
    case 0:
      m0 = IOMUX->SECCFG.PINCM[PB15INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB16INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB17INDEX];
      break;
    case 1:
      m0 = IOMUX->SECCFG.PINCM[PB16INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB17INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB18INDEX];
      break;
    case 2:
      m0 = IOMUX->SECCFG.PINCM[PB17INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB18INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB19INDEX];
      break;
    case 3:
      m0 = IOMUX->SECCFG.PINCM[PB18INDEX];
      m1 = IOMUX->SECCFG.PINCM[PB19INDEX];
      m2 = IOMUX->SECCFG.PINCM[PB20INDEX];
      break;
  }
  if((m0 != 0x00040081)||(m1 != 0x00040081)||(m2 != 0x00040081)){
    UART_OutString("PINCM register for the Sensor pins should be 0x00040081.\n\r");
    Fail = 1;
    return;
  }
}
uint32_t ReadSouth(void){ // bits 2-0
  switch(SouthMode){
    case 0:
      return GPIOB->DOUT31_0&0x07;
    case 1:
      return (GPIOB->DOUT31_0&0x0E)>>1;
    case 2:
      return (GPIOB->DOUT31_0&0x1C)>>2;
  }
  return 0;
}
uint32_t ReadSensor(void){ // bits 2-0
  switch(SensorMode){
  case 0:
    return (GPIOB->DIN31_0&0x38000)>>15;
  case 1:
    return (GPIOB->DIN31_0&0x70000)>>16;
  case 2:
    return (GPIOB->DIN31_0&0xE0000)>>17;
  case 3:
    return (GPIOB->DIN31_0&0x1C0000)>>18;
  }
  return 0;
}
uint32_t ReadWest(void){ // bits 5-3
  switch(WestMode){
    case 0:
      return (GPIOB->DOUT31_0&0x1C0)>>3;
    case 1:
      return (GPIOB->DOUT31_0&0x380)>>4;
  }
  return 0;
}
uint32_t ReadWalk(void){ //bits 8-6
  return (GPIOB->DOUT31_0 & BLUE)>> (22-8)| // move from 22 to 8
         (GPIOB->DOUT31_0 & RED)>> (26-6)| // move from 26 to 6
         (GPIOB->DOUT31_0 & GREEN)>> (27-7); // move from 27 to 7
}
#define W_RED 0x20      // west red
#define W_YELLOW 0x10   // west yellow
#define W_GREEN 0x08    // west green
#define S_RED 0x04     // north red
#define S_YELLOW 0x02  // north yellow
#define S_GREEN 0x01   // north green
#define R_R (S_RED+W_RED)    // south=red,   west=red     0x24
#define Y_R (S_YELLOW+W_RED) // south=yellow,west=red     0x22
#define G_R (S_GREEN+W_RED)  // south=green, west=red     0x21
#define R_G (S_RED+W_GREEN)  // south=red,   west=green   0x0C
#define R_Y (S_RED+W_YELLOW) // south=red,   west=yellow  0x14
#define Wa_GO  (0x1C0)    // white
#define Wa_RED (0x40)    // red
#define Wa_OFF 0x00         // off or dark
#define NEXTNUM 4
struct graderstate{
  char Name[12];
  uint32_t PossibleNext[NEXTNUM];
};
typedef const struct graderstate graderstate_t;
const uint32_t validPatterns[7]={
  (R_R+Wa_RED),
  (G_R+Wa_RED),
  (Y_R+Wa_RED),
  (R_G+Wa_RED),
  (R_Y+Wa_RED),
  (R_R+Wa_GO),
  (R_R+Wa_OFF)};
#define stop 0
#define goSo 1
#define waSo 2
#define goWe 3
#define waWe 4
#define goWa 5
#define waWa 6
const char PatternProblem[2][100]={
  "When all inputs true, ... South, Walk, West, South, Walk, West, South, Walk, West, ...", // PatternProblem=0
  "When all inputs true, ... South, West, Walk, South, West, Walk, South, West, Walk, ..."  // PatternProblem=1
};
const char SouthProblem[3][80]={
  "Option A, South,  connect Red=PB2, Yellow=PB1, Green=PB0",        // SouthMode=0
  "Option B, South,  connect Red=PB3, Yellow=PB2, Green=PB1",        // SouthMode=1
  "Option C, South,  connect Red=PB4, Yellow=PB3, Green=PB2"         // SouthMode=2
};
const char WestProblem[2][80]={
  "Option X, West,   connect Red=PB8, Yellow=PB7, Green=PB6",        // WestMode=0
  "Option Y, West,   connect Red=PB9, Yellow=PB8, Green=PB7"        // WestMode=1
};
const char SensorProblem[4][80]={
  "Option 0, Sensor, connect Walk=PB17, South=PB16, West=PB15",     // SensorMode=0
  "Option 1, Sensor, connect Walk=PB18, South=PB17, West=PB16",     // SensorMode=1
  "Option 2, Sensor, connect Walk=PB19, South=PB18, West=PB17",     // SensorMode=2
  "Option 3, Sensor, connect Walk=PB20, South=PB19, West=PB18"     // SensorMode=3
};
uint32_t LookingFor,NeedState,WrongState,LookingFlag;
const uint32_t Patterns[2][3]={
  {goSo,goWa,goWe},
  {goSo,goWe,goWa}
};
graderstate_t pat[7]={
  {"All stop", {goSo,goWe,goWa,waWa}},
  {"Go South", {waSo,7,7,7}},
  {"Wait South", {stop,7,7,7}},
  {"Go West", {waWe,7,7,7}},
  {"Wait West", {stop,7,7,7}},
  {"Go Walk", {stop,waWa,7,7}},
  {"Wait Walk", {stop,7,7,7}}
};
uint32_t GetState(uint32_t out){
  if(out!=0){
    int i;
    for(i=0;i<7;i++){
      if(validPatterns[i] == out){
        return i;
      }
    }
    UART_OutString("Illegal output\n\r");
    Fail = 1;
  }
  return 8;
}
uint32_t LastIn,LastOut,LastState; // previous input output state
// runs periodically in timer G7 ISR
uint32_t in,out,state;
void TIMG7_IRQHandler(void){
  if((TIMG7->CPU_INT.IIDX) == 1){ // this will acknowledge
    if(Fail){
      UART_OutString("Halted, Score= ");
      UART_OutUDec(Score);OutCRLF();
      NVIC->ICER[0] = 1 << 20; // disarm TIMG7 interrupt
    }
    Time++;
    if((GPIOB->DOUT31_0&0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
      Fail = 1;
      return;
    }
    if(Time > 10){
      in = ReadSensor();
      out = ReadSouth()|ReadWest()|ReadWalk();
      if((in == LastIn)&&(out == LastOut)){ // filter for glitch
        state = GetState(out);
        if(state == 8){
          return; // fail
        }
        if(in == 7){
          if(LookingFlag){
            UART_OutString("Looking for ");
            NeedState = Patterns[PatternMode][LookingFor];
            UART_OutString((char *)pat[NeedState].Name);
            LookingFlag = 0;
            if(LookingFor == 1){
              WrongState = Patterns[PatternMode][2];
            }else{
              WrongState = 8; // only check pattern when looking for 1, but find 2
            }
            OutCRLF();
          }
          if(LastState == 8){
            LastState = state;          // initial state, no transition
          }else if(state != LastState){ // new state
           int j; int err=1;
            for(j=0;j<NEXTNUM;j++){
              if(pat[LastState].PossibleNext[j] == state){
                UART_OutString((char *)pat[state].Name);
                OutCRLF();
                err = 0;
                LastState = state;
                if(LastState == WrongState){
                  UART_OutString("Pattern error\n\r");
                  Fail = 1;
                  return;
                }
                if(LastState == NeedState){
                  UART_OutString("Found ");
                  UART_OutString((char *)pat[NeedState].Name);
                  if(LookingFor<2){
                    LookingFor++;
                    UART_OutString(", Score= ");
                    Score = Score+7;
                    UART_OutUDec(Score);
                    OutCRLF();
                    LookingFlag =1;
                  }
                  else{
                   UART_OutString(", Finished, Score= ");
                    Score = Score+7;
                    UART_OutUDec(Score);
                    OutCRLF();
                    NVIC->ICER[0] = 1 << 20; // disarm TIMG7 interrupt
                  }
                }
              }
            }
            if(err){
              UART_OutString("bad from ");
              UART_OutString((char *)pat[LastState].Name);
              UART_OutString(" to ");
              UART_OutString((char *)pat[state].Name);
              OutCRLF();
              Fail = 1;
            }
          }
        }
      }
      LastIn = in;
      LastOut = out;
    }
  }
}
void Grader_Init(void){
  // configures PB5 as output to make sure student code is friendly
  IOMUX->SECCFG.PINCM[PB5INDEX] = 0x00000081; // PB5 regular GPIO output
  GPIOB->DOE31_0 |= 0x20;    // enable output PB5
  GPIOB->DOUT31_0 |= 0x20;   // output is high
}
void Lab4Grader(int mode){int flag;
// assumes 80 MHz, and LaunchPad_Init
  UART_Init();

  M = 4;
  Score=0;
  LastState = 8;
  WrongState = 8;
  LookingFor = 0;
  LookingFlag = 1;
  LastOut = 0;
  __enable_irq(); // grader runs in background using interrupts
  UART_OutString("Lab 4, Spring 2024\n\r");
  if((strcmp(EID1,"abc123"))&&strcmp(EID2,"abc123")){
    for(int i=0; EID1[i]; i++){
      M = M*setUpper(EID1[i]);
    }
    for(int i=0; EID2[i]; i++){
      M = M*setUpper(EID2[i]);
    }
/*WestMode    randomly assigns 0 to 1
  SouthMode   randomly assigns 0 to 2
  SensorMode  randomly assigns 0 to 3
  PatternMode randomly assigns 0 to 1*/
    WestMode = (Random32()>>29)&1;
    WestMode = 0; // force for debugging
    SouthMode = (Random32()>>24)%3;
    SouthMode = 0; // force for debugging
    SensorMode = (Random32()>>26)&3;
    SensorMode = 0; // force for debugging
    PatternMode = (Random32()>>31); // 0,1
    PatternMode =0; // force for debugging
    UART_OutString("EID1=");
    for(int i=0; EID1[i]; i++){
      UART_OutChar(setUpper(EID1[i]));
    }
    UART_OutString(", EID2=");
    for(int i=0; EID2[i]; i++){
      UART_OutChar(setUpper(EID2[i]));
    }
    OutCRLF();
    UART_OutString((char*)SouthProblem[SouthMode]); OutCRLF();
    UART_OutString((char*)WestProblem[WestMode]); OutCRLF();
    UART_OutString((char*)SensorProblem[SensorMode]); OutCRLF();
    UART_OutString((char*)PatternProblem[PatternMode]); UART_OutString(" ....."); OutCRLF();
    flag = 1;
  }else{
    flag = 0;
    UART_OutString("\n\rPlease enter your two EIDs into ECE319K_Lab4main.c\n\r");
  }
  if(mode==0)return;
  Fail = 0;

  CheckInitialization();
  if(Fail == 1) {
    UART_OutString("Grader halted.\n\r");
    return;
  }
  Score += 4;
  UART_OutString("Grader started, Activate all three inputs.\n\r");
  Time = 0;
  TimerG7_IntArm(10000,80,3); // 100Hz, lowest priority

}


