// Lab6Grader.c
// Runs on MSPM0G3507
// Periodic timer interrupt data collection
// PLL turned on at 80 MHz

// Jonathan Valvano, Daniel Valvano
// 12/28/2023


#include "Lab6Grader.h"
#include <stdint.h>
#include <string.h>
#include "../inc/Clock.h"
#include <ti/devices/msp/msp.h>
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "StringConversion.h"

//---------------------OutCRLF---------------------
// Busy-wait version, Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

void TimerG7_Stop(void){
  TIMG7->CPU_INT.IMASK = 0; // disarm
}

extern char EID1[16];  // student's EID
extern char EID2[16];  // student's EID
static int Fail=0;
static uint32_t Score;
int testSPIOutCommand(uint32_t command, 
  uint32_t *statusReg, 
  uint32_t *dataReg,
  uint32_t *gpioReg);
uint32_t GraderStatus;
uint32_t GraderData;
uint32_t GraderGPIO;
uint32_t GraderTime;
int testSPIOutData(uint32_t data, 
  uint32_t *statusReg, 
  uint32_t *dataReg,
  uint32_t *gpioReg);
uint32_t whichbit;
void TIMG7_IRQHandler(void){
  if((TIMG7->CPU_INT.IIDX) == 1){ // this will acknowledge
    GraderTime++;
    if(GraderTime == 4){
      if(whichbit){
        GraderStatus &= ~0x10; // clear bit 4
      }else{
        GraderStatus |= 0x02; // set bit 1
      }
    }
    if(GraderTime > 10){
      Fail=1;
      TimerG7_Stop();
      UART_OutString("*** bug! It never returned\n\rScore =0\n\r");
      UART_OutString("Fatal error that must be fixed first\n\r");
      while(1){
        //crash
      }
    }
  }
}

void GradeSPIOutCommand(void){
  GraderTime = 0;
  GraderStatus = 0xF8; // bit4 initially high
  GraderGPIO = 0x49;
  UART_OutString("Testing SPIOutCommand: ");
  whichbit = 1; //ClearBit4
  TimerG7_IntArm(100,80,2); // 80MHz/100/80 = 10kHz
  __enable_irq();;
  int result = testSPIOutCommand(0x05,&GraderStatus,&GraderData,&GraderGPIO);
  TimerG7_Stop();
  __disable_irq();
  if(result == 1){
    UART_OutString("*** stack error\n\r");
  }else if(result == 2){
    UART_OutString("*** AAPCS error\n\r");
  }else if(GraderGPIO != 0x2000){
    UART_OutString("*** GPIO D/C error\n\r");
    result = 3;
  }else if(GraderData != 0x05){
    UART_OutString("*** command error\n\r");
    result = 4;
  }else if(GraderStatus != 0xE8){
    UART_OutString("***, can't write to status \n\r");
    result = 5;
  }else if(GraderTime < 4){
    UART_OutString("*** did not wait for busy (bit 4) to be low\n\r");
    result = 6;
  }
  if(result == 0){
    UART_OutString("ok, Score =");
    Score += 8;
    UART_OutUDec(Score); OutCRLF();
  }
  Fail = Fail|result;
}


void GradeSPIOutData(void){
  GraderTime = 0;
  GraderStatus = 0xF8; // bit1 initially low
  GraderGPIO = 0x09;
  UART_OutString("Testing SPIOutData: ");
  whichbit = 0; //SetBit1
  TimerG7_IntArm(100,80,2); // 80MHz/100/80 = 10kHz
  // run SetBit1
  __enable_irq();
  int result = testSPIOutData(0x35,&GraderStatus,&GraderData,&GraderGPIO);
  TimerG7_Stop();
  __disable_irq();
  if(result == 1){
    UART_OutString("*** stack error\n\r");
  }else if(result == 2){
    UART_OutString("*** AAPCS error\n\r");
  }else if(GraderGPIO != 0x2000){
    UART_OutString("*** GPIO D/C error\n\r");
    result = 3;
  }else if(GraderData != 0x35){
    UART_OutString("*** data error\n\r");
    result = 4;
  }else if(GraderStatus != 0xFA){
    UART_OutString("***, can't write to status \n\r");
    result = 5;
  }else if(GraderTime < 4){
    UART_OutString("*** did not wait for TNF (bit 1) to be high\n\r");
    result = 6;
  }
  if(result == 0){
    UART_OutString("ok, Score =");
    Score += 7;
    UART_OutUDec(Score); OutCRLF();
  }
  Fail = Fail|result;
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
//-----------------------testDec2String-----------------------
// Convert a 32-bit number in unsigned decimal format
// Input: n is input number
//        string is an empty string
// Output: none
// n=0,    then create "0"
// n=3,    then create "3"
// n=89,   then create "89"
// n=123,  then create "123"
// n=9999, then create "9999"
int testDec2String(uint32_t n, char string[16]);
void myDec2String(uint32_t n, char string[16]);
char myMessage[16];
char yourMessage[16];

void GradeDec2String(void){ uint32_t n; int result;
  UART_OutString("Testing OutDec: ");
  for(int n=0; n<=65535; n++){
    yourMessage[15] = 0;
    result = testDec2String(n,yourMessage);
    if(result == 1){
      UART_OutString("*** stack error\n\r");
      Fail = 1;
      return;
    }else if(result == 2){
      UART_OutString("*** AAPCS error\n\r");
      Fail = 1;
      return;
   }else{
      myDec2String(n,myMessage);
      if(strcmp(myMessage,yourMessage)){
        UART_OutString("*** correct= ");
        UART_OutString(myMessage);
        UART_OutString(" your= ");
        UART_OutString(yourMessage);
        UART_OutString("\n\rScore =");
        UART_OutUDec(Score); OutCRLF();
        return; // non fatal error
      }
    }
  }
  UART_OutString("ok, Score =");
  Score += 10;
  UART_OutUDec(Score); OutCRLF();
}
//-----------------------testFix2String-----------------------
// Output a 32-bit number in unsigned fixed-point format, 0.001 format
// Input: n is input number
//        string is an empty string// Input: none
// Output: none
// n=0,    then create "0.000"
// n=3,    then create "0.003"
// n=89,   then create "0.089"
// n=123,  then create "0.123"
// n=9999, then create "9.999"
// n>9999, then create "*.***"
/*
int testFix2String(uint32_t n, char string[16]);
void myFix2String(uint32_t n, char string[16]);
void GradeFix2String(void){ uint32_t n; int result;
  UART_OutString("Testing Fix2String: ");
  for(n=0;n<20000;n++){
    yourMessage[15] = 0;
    result = testFix2String(n,yourMessage);
    if(result == 1){
      UART_OutString("*** stack error\n\r");
      Fail = 1;
      return;
    }else if(result == 2){
      UART_OutString("*** AAPCS error\n\r");
      Fail = 1;
      return;
   }else{
      myFix2String(n,myMessage);
      if(strcmp(myMessage,yourMessage)){
        UART_OutString("*** correct= ");
        UART_OutString(myMessage);
        UART_OutString(" your= ");
        UART_OutString(yourMessage);
        UART_OutString("\n\rScore =");
        UART_OutUDec(Score); OutCRLF();
        return; // non fatal error
      }
    }
  }
  UART_OutString("ok, Score =");
  Score += 5;
  UART_OutUDec(Score); OutCRLF();
}
*/

char setUpper(char in){
  if((in >= 'a')&&(in <= 'z')){
    return in-0x20;
  }
  return in;
}


// ************Lab6Grader*****************
// Initialize grader, triggered by periodic timer
// Bus clock frequency will be 80 MHz
// Inputs: mode
// mode = 1 to test SPIOutCommand, no grading
// mode = 2 to test SPIOutData, no grading
// mode = 3 to test GradeDec2String, no grading
// mode = 4 to test all three with grading
// Outputs: none
void Lab6Grader(int mode){int flag;
// assumes 80 MHz, and LaunchPad_Init
  UART_Init();
  M = 4;
  Score=0;
  __enable_irq(); // grader runs in background using interrupts
  UART_OutString("Lab 6, Spring 2024, ");
  if((strcmp(EID1,"abc123"))&&strcmp(EID2,"abc123")){
    for(int i=0; EID1[i]; i++){
      M = M*setUpper(EID1[i]);
    }
    for(int i=0; EID2[i]; i++){
      M = M*setUpper(EID2[i]);
    }
    
    UART_OutString("EID1=");
    for(int i=0; EID1[i]; i++){
      UART_OutChar(setUpper(EID1[i]));
    } 
    UART_OutString(", EID2=");
    for(int i=0; EID2[i]; i++){
      UART_OutChar(setUpper(EID2[i]));
    } 
    OutCRLF();
   }else{
    UART_OutString("\n\rPlease enter your two EIDs into Lab6Main.c\n\r");
    return;
  }  
  Fail = 0;
  if(mode == 1){
    GradeSPIOutCommand();
  }else if(mode == 2){
    GradeSPIOutData();
  }else if(mode == 3){
    GradeDec2String();
  }else if(mode == 4){
    GradeSPIOutCommand();
    GradeSPIOutData();
    GradeDec2String();
  //  GradeFix2String();
  }
  if(Fail){
    UART_OutString("Fatal error that must be fixed first\n\r");
  }
}


