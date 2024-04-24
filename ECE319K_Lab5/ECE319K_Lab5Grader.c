/* ECE319K_Lab5Grader.c
 * Jonathan Valvano
 * December 28, 2023
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
// 5-bit binary-weighted DAC connected to PB4-PB0
// 4-bit keyboard connected to PB19-PB16
// logic analyzer pins PB17(in), PB16 (in), PB4-PB0(out)
// analog scope pin PB20, channel 6

extern const char EID1[20];
extern const char EID2[20];
int NotesMode=0; // randomly assigns 0 to 7
static int Fail=0;
static uint32_t Time=0;
static uint32_t Score=0;
const char NotesProblem[8][80]={
  "Key0=329.6, Key1=415.3, Key2=493.9, Key3=554.4 Hz",        // NotesMode=0
  "Key0=311.1, Key1=392.0, Key2=466.2, Key3=523.3 Hz",        // NotesMode=1
  "Key0=293.7, Key1=370.0, Key2=440.0, Key3=493.9 Hz",        // NotesMode=2
  "Key0=277.2, Key1=349.2, Key2=415.3, Key3=466.2 Hz",        // NotesMode=3
  "Key0=261.6, Key1=329.6, Key2=392.0, Key3=440.0 Hz",        // NotesMode=4
  "Key0=246.9, Key1=311.1, Key2=370.0, Key3=415.3 Hz",        // NotesMode=5
  "Key0=233.1, Key1=293.7, Key2=349.2, Key3=392.0 Hz",        // NotesMode=6
  "Key0=220.0, Key1=277.2, Key2=329.6, Key3=370.0 Hz"         // NotesMode=7
};
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
void OutNotesProblem(void){
  UART_OutString((char*)NotesProblem[NotesMode]); OutCRLF();
}
const uint32_t NotesPeriod[8][4]={ // interrupt periods
//{15169, 12039, 10124, 9019},
//{16071, 12755, 10726, 9556},
//{17026, 13514, 11364, 10124},
//{18039, 14317, 12039, 10726},
//{19111, 15169, 12755, 11364},
//{20248, 16071, 13514, 12039},
//{21452, 17026, 14317, 12755},
//{22727, 18039, 15169, 13514}
//};
//Bus clock  Pnts/wave   Bus/n
//80000000    8          10000000
// Bus/8/Frequency = 10000000/Frequency
{30337, 24079, 20248, 18039}, // assumes 8 samples/wave
{32141, 25511, 21452, 19111},
{34052, 27027, 22727, 20248},
{36077, 28635, 24079, 21452},
{38223, 30337, 25511, 22727},
{40495, 32141, 27027, 24079},
{42903, 34052, 28635, 25511},
{45455, 36077, 30337, 27027}
};


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
void Grader_Init(void){
  // configures PB5 as output to make sure student code is friendly
  IOMUX->SECCFG.PINCM[PB5INDEX] = 0x00000081; // PB5 regular GPIO output
  GPIOB->DOE31_0 |= 0x20;    // enable output PB5
  GPIOB->DOUT31_0 |= 0x20;   // output is high
}
void CheckInitialization(void){
  uint32_t m0,m1,m2,m3,m4,doe;
  UART_OutString("Initialization, ");
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
    Fail = 1;
    return;
  }
  m0 = IOMUX->SECCFG.PINCM[PB0INDEX]; // regular GPIO output
  m1 = IOMUX->SECCFG.PINCM[PB1INDEX]; // regular GPIO output
  m2 = IOMUX->SECCFG.PINCM[PB2INDEX]; // regular GPIO output
  m3 = IOMUX->SECCFG.PINCM[PB3INDEX]; // regular GPIO output
  m4 = IOMUX->SECCFG.PINCM[PB4INDEX]; // regular GPIO output
  doe = GPIOB->DOE31_0 &0x01F;      // enable output PB4,3,2,1,0
  if((m0 != 0x00000081)||(m1 != 0x00000081)||(m2 != 0x00000081)
          ||(m3 != 0x00000081)||(m3 != 0x00000081)){
    UART_OutString("PINCM register for the DAC pins should be 0x00000081.\n\r");
    Fail = 1;
    return;
  }
  if(doe != 0x1F){
    UART_OutString("GPIOB->DOE31_0 register for the DAC pins should be set.\n\r");
    Fail = 1;
    return;
  }
  m0 = IOMUX->SECCFG.PINCM[PB16INDEX];// regular GPIO input
  m1 = IOMUX->SECCFG.PINCM[PB17INDEX]; // regular GPIO input
  m2 = IOMUX->SECCFG.PINCM[PB18INDEX]; // regular GPIO input
  m3 = IOMUX->SECCFG.PINCM[PB19INDEX]; // regular GPIO input
  if((m0 != 0x00040081)||(m1 != 0x00040081)||(m2 != 0x00040081)||(m3 != 0x00040081)){
    UART_OutString("PINCM register for the keyboard pins should be 0x00040081.\n\r");
    Fail = 1;
    return;
  }
  UART_OutString("good, Score=4\n\r");
  Score = 4;
  Fail = 0;
}
uint32_t ReadInput(void){
  if(Fail == 0){
    return (GPIOB->DIN31_0>>16)&0x0F;
  }
  return 0;
}
uint32_t ReadOutput(void){
  if(Fail == 0){
    return GPIOB->DOUT31_0&0x1F;
  }
  return 0;
}

uint32_t LastIn,State,Mask,SampleFlag;   // previous input state
uint32_t Last16ADCresults[16];     // average the 64 most recent ADC conversions, filled in ADC1_In()
uint32_t Last16ADCindex = 0;       // index into Last16ADCresults array
// These values are multiplied by the data to calculate the FFT.
// The final result will be 17 times too large, due to integer math.
// sqrt(2)/2 = 0.7071 and 12/17 = 0.7059
const int32_t RMULTIPLIERS[8] = {17, 12, 0, -12, -17, -12, 0, 12};
const int32_t IMULTIPLIERS[8] = {0, -12, -17, -12, 0, 12, 17, 12};


// calculate FFT with frequency bins of 1/(16*newADCperiod), and
// return 0 to 6 points earned in the test.
uint32_t FFT(void){
  int32_t rbin0=0, rbin1=0, rbin2=0, rbin3=0, rbin4=0, rbin5=0, rbin6=0, rbin7=0;
  int32_t ibin1=0, ibin2=0, ibin3=0, ibin4=0, ibin5=0, ibin6=0, ibin7=0;
  int32_t mag0, mag1, mag2, mag3, mag4, mag5, mag6, mag7;
  int32_t i;
  // restore settings for Timer5 and ADC1 to what they were before this
  // special Background Test began

  // calculate the magnitude of the FFT using some shortcuts
  // bin 0: 0 to 1/16*TargetFrequency
  //        will be large due to necessary ~1.65 V offset
  // bin 1: 1/16*TargetFrequency to 2/16*TargetFrequency
  //        should be largest bin by far for sine wave of TargetFrequency
  // bins 2-6: n/16*TargetFrequency to (n+1)/16*TargetFrequency
  //        should all be much smaller than bin 1 for sine wave
  // bin 7: 7/16*TargetFrequency to 8/16*TargetFrequency
  //        complex conjugate equal in magnitude to bin 1
  // assumptions: the 0 to 3 V ADC causes clipping but it doesn't matter
  // make sure that 16 samples have been taken

  for(i=0; i<16; i=i+1){
    // calculate bin 0
    rbin0 = rbin0 + Last16ADCresults[i];
    // calculate bin 1
    rbin1 = rbin1 + Last16ADCresults[i]*RMULTIPLIERS[i&0x7];
    ibin1 = ibin1 + Last16ADCresults[i]*IMULTIPLIERS[i&0x7];
    // calculate bin 2
    rbin2 = rbin2 + Last16ADCresults[i]*RMULTIPLIERS[(2*i)&0x7];
    ibin2 = ibin2 + Last16ADCresults[i]*IMULTIPLIERS[(2*i)&0x7];
    // calculate bin 3
    rbin3 = rbin3 + Last16ADCresults[i]*RMULTIPLIERS[(3*i)&0x7];
    ibin3 = ibin3 + Last16ADCresults[i]*IMULTIPLIERS[(3*i)&0x7];
    // calculate bin 4
    rbin4 = rbin4 + Last16ADCresults[i]*RMULTIPLIERS[(4*i)&0x7];
    ibin4 = ibin4 + Last16ADCresults[i]*IMULTIPLIERS[(4*i)&0x7];
    // calculate bin 5
    rbin5 = rbin5 + Last16ADCresults[i]*RMULTIPLIERS[(5*i)&0x7];
    ibin5 = ibin5 + Last16ADCresults[i]*IMULTIPLIERS[(5*i)&0x7];
    // calculate bin 6
    rbin6 = rbin6 + Last16ADCresults[i]*RMULTIPLIERS[(6*i)&0x7];
    ibin6 = ibin6 + Last16ADCresults[i]*IMULTIPLIERS[(6*i)&0x7];
    // calculate bin 7
    rbin7 = rbin7 + Last16ADCresults[i]*RMULTIPLIERS[(7*i)&0x7];
    ibin7 = ibin7 + Last16ADCresults[i]*IMULTIPLIERS[(7*i)&0x7];
  }
  // calculate the magnitude^2 = (real^2 + imaginary^2)
  // before squaring, divide by 68 to prevent overflow
  mag0 = (rbin0/68)*(rbin0/68);
  mag1 = (rbin1/68)*(rbin1/68) + (ibin1/68)*(ibin1/68);
  mag2 = (rbin2/68)*(rbin2/68) + (ibin2/68)*(ibin2/68);
  mag3 = (rbin3/68)*(rbin3/68) + (ibin3/68)*(ibin3/68);
  mag4 = (rbin4/68)*(rbin4/68) + (ibin4/68)*(ibin4/68);
  mag5 = (rbin5/68)*(rbin5/68) + (ibin5/68)*(ibin5/68);
  mag6 = (rbin6/68)*(rbin6/68) + (ibin6/68)*(ibin6/68);
  mag7 = (rbin7/68)*(rbin7/68) + (ibin7/68)*(ibin7/68);
  // evaluate the magnitude^2 and award points
  // for perfect sine wave:
  // mag0 =      14,884
  // mag1 =     950,948
  // mag2 =           4
  // mag3 =         890
  // mag4 =           4
  // mag5 =         890
  // mag6 =           4
  // mag7 =     950,948
  if((mag0 >   100000) ||
     (mag1 <   200000) ||
     (mag2 >   100000) ||
     (mag3 >   100000) ||
     (mag4 >   100000)){
    return 0;                       // at least one bad magnitude; award no points
  }
  return 6;
}
#define ADC_In(adc12) ((adc12)->ULLMEM.MEMRES[0])
ADC12_Regs *TExaSadc;
uint32_t test=0;

#define ADCVREF_INT  0x200
// choose internal 2.5V reference for accuracy
#define ADCVREF_EXT  0x100
// external reference not tested
#define ADCVREF_VDDA 0x000
//choose power line 3.3V reference for 0 to 3.3V range

// Bus frequency set by main program
// continuous sampling, 8 bit mode
void ADC_Init(ADC12_Regs *adc12, uint32_t channel, uint32_t reference){
    // Reset ADC and VREF
    // RSTCLR
    //   bits 31-24 unlock key 0xB1
    //   bit 1 is Clear reset sticky bit
    //   bit 0 is reset ADC
  TExaSadc = adc12;
  adc12->ULLMEM.GPRCM.RSTCTL = (uint32_t)0xB1000003;
  if(reference == ADCVREF_INT){
    VREF->GPRCM.RSTCTL = (uint32_t)0xB1000003;
  }
    // Enable power ADC and VREF
    // PWREN
    //   bits 31-24 unlock key 0x26
    //   bit 0 is Enable Power
  adc12->ULLMEM.GPRCM.PWREN = (uint32_t)0x26000001;
  if(reference == ADCVREF_INT){
    VREF->GPRCM.PWREN = (uint32_t)0x26000001;
  }
  Clock_Delay(24); // time for ADC and VREF to power up
  adc12->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // SYSOSC
  // bits 31-24 key=0xA9
  // bit 5 CCONSTOP= 0 not continuous clock in stop mode
  // bit 4 CCORUN= 0 not continuous clock in run mode
  // bit 1-0 0=SYSOSC,1=MCLK,2=HFCLK
  if(Clock_Freq() > 32000000){
    adc12->ULLMEM.CLKFREQ = 7; // 7 is 40 MHz
  }else{
    adc12->ULLMEM.CLKFREQ = 5; //5 is 32MHz
  }
  adc12->ULLMEM.CTL0 = 0x03010000;
  // bits 26-24 = 011 divide by 8
  // bit 16 PWRDN=1 for manual, =0 power down on completion, if no pending trigger
  // bit 0 ENC=0 disable (1 to 0 will end conversion), =1 for enable
  adc12->ULLMEM.CTL1 = 0x00020000;
  // bits 30-28 =0  no shift
  // bits 26-24 =0  no averaging
  // bit 20 SAMPMODE=0 high phase
  // bits 17-16 CONSEQ=00 ADC at start will be sampled once, 10 for repeated sampling
  // bit 8 SC=0 for stop, =1 to software start
  // bit 0 TRIGSRC=0 software trigger
  adc12->ULLMEM.CTL2 = 0x00000004;
  // bits 28-24 ENDADD (which  MEMCTL to end)
  // bits 20-16 STARTADD (which  MEMCTL to start)
  // bits 15-11 SAMPCNT (for DMA)
  // bit 10 FIFOEN=0 disable FIFO
  // bit 8  DMAEN=0 disable DMA
  // bits 2-1 RES=00 for 12 bit (=01 for 10bit,=10for 8-bit)
  // bit 0 DF=0 unsigned formant (1 for signed, left aligned)
  adc12->ULLMEM.MEMCTL[0] = reference+channel;
  // bit 28 WINCOMP=0 disable window comparator
  // bit 24 TRIG trigger policy, =0 for auto next, =1 for next requires trigger
  // bit 20 BCSEN=0 disable burn out current
  // bit 16 = AVGEN =0 for no averaging
  // bit 12 = STIME=0 for SCOMP0
  // bits 9-8 VRSEL = 10 for internal VREF,(00 for VDDA)
  // bits 4-0 channel = 0 to 7 available
  adc12->ULLMEM.SCOMP0 = 0; // 8 sample clocks
//  adc12->ULLMEM.GEN_EVENT.ICLR |= 0x0100; // clear flag MEMCTL[0]
  adc12->ULLMEM.GEN_EVENT.IMASK = 0; // no interrupt
  if(reference == ADCVREF_INT){
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
}
// start continuous sampling 8-bit ADC
void ADC_Start(ADC12_Regs *adc12){
  adc12->ULLMEM.CTL0 |= 0x00000001; // enable conversions
  adc12->ULLMEM.CTL1 |= 0x00000100; // start ADC
}
void (*PeriodicTask2)(void);   // user function
// runs periodically in timer G7 ISR
void TIMG7_IRQHandler(void){uint32_t in;
  if((TIMG7->CPU_INT.IIDX) == 1){ // this will acknowledge
    (*PeriodicTask2)();               // execute user task
  }
}
#define UART_Out(data) (UART0->TXDATA = data)

void LogicAnalyzer(void){  // called 10k/sec
  UART_Out(0x80|(GPIOB->DOUT31_0&0x01F)|((GPIOB->DIN31_0>>11)&0x060)); // 7-bit digital data at 10 kHz
}
void Scope(void){  // called 10k/sec
  UART_Out(ADC_In(TExaSadc)); // 8-bit analog data at 10 kHz
}
// assumes 80MHz
void lab5grader(void){uint32_t in;
  if(Fail){
    UART_OutString("Halted, Score= ");
    UART_OutUDec(Score);OutCRLF();
    NVIC->ICER[0] = 1 << 20; // disarm TIMG7 interrupt
  }
  if((GPIOB->DOUT31_0&0x20) == 0){
    UART_OutString("DOUT not friendly\n\r");
    Fail = 1;
    return;
  }
  if(SampleFlag == 1){
    if(Last16ADCindex<16){
      Last16ADCresults[Last16ADCindex] = ADC_In(TExaSadc);
    }else{
      Last16ADCresults[Last16ADCindex&0x0F] += ADC_In(TExaSadc);
    }
    Last16ADCindex++;
    if(Last16ADCindex>=64){
      SampleFlag = 2;
      TimerG7_IntArm(40000,20,1); // 100 Hz
    }
    return;
  }
  Time++;
  if(Time == 10){
    CheckInitialization();
    if(Fail == 0){
      UART_OutString("Activate piano Key0");
      State = 0; //
      Mask = 1;  // looking for Key0
      SampleFlag = 0; // start looking
      return;
    }
  }
  if(Time > 10){
    in = ReadInput();
    if(in == LastIn){
      if((in == Mask)&&(SampleFlag ==0)){
        TimerG7_IntArm(NotesPeriod[NotesMode][State],1,1); // 8 times expected freq
        SampleFlag = 1; // start looking
        Last16ADCindex = 0;
      }
    }
    if(SampleFlag == 2){
        // do FFT
      uint32_t result = FFT();
      if(result){
        Score = Score+result;
        if(Score>25) Score = 25;
        UART_OutString(" Good. Score=");
        UART_OutUDec(Score); OutCRLF();
      }else{
        UART_OutString(" Waveform wrong frequency or not sinusoidal\n\r");
      }
      State++;
      Mask = Mask<<1;
      if(State < 4){
        SampleFlag = 0;
        UART_OutString("Activate piano Key");
        UART_OutUDec(State);
      }else{
        UART_OutString("Done. Score=");
        UART_OutUDec(Score); OutCRLF();
        SampleFlag = 3;
        TIMG7->CPU_INT.IMASK = 0; // stop
      }
    }
    LastIn = in;
  }
}


void Lab5Grader(int Phase){
// assumes 80 MHz, and LaunchPad_Init
  UART_Init();
  M = 5;
  Score=0;

  __enable_irq(); // grader runs in background using interrupts
  UART_OutString("Lab 5, Spring 2024\n\r");
  if((strcmp(EID1,"abc123"))&&strcmp(EID2,"abc123")){
    for(int i=0; EID1[i]; i++){
      M = M*setUpper(EID1[i]);
    }
    for(int i=0; EID2[i]; i++){
      M = M*setUpper(EID2[i]);
    }
    NotesMode = (Random32()>>29); // 0,1,...7
    //NotesMode = 7; // force for debugging

    if(Phase == 0){ // 0 for info
      UART_OutString("Student EID1= ");
      for(int i=0; EID1[i]; i++){
        UART_OutChar(setUpper(EID1[i]));
      }
      UART_OutString(", EID2=");
      for(int i=0; EID2[i]; i++){
        UART_OutChar(setUpper(EID2[i]));
      }
      OutCRLF();
      UART_OutString((char*)NotesProblem[NotesMode]); OutCRLF();
      return;
    }
    if(Phase == 1){//  1 debug with logic analyzer
      TExaSadc = 0;
      PeriodicTask2 = LogicAnalyzer;
      TimerG7_IntArm(8000,1,3); // 10kHz
      return;
    }
    if(Phase == 2){//  2 debug with scope,
      ADC_Init(ADC0,6,ADCVREF_VDDA); // PB20, 0 to 3.3V range
      ADC_Start(ADC0);
      PeriodicTask2 = Scope;
      TimerG7_IntArm(8000,1,3); // 10kHz
    }
    if(Phase == 3){//  3 for grade
      UART_OutString("Student EID1= ");
      for(int i=0; EID1[i]; i++){
        UART_OutChar(setUpper(EID1[i]));
      }
      UART_OutString(", EID2=");
      for(int i=0; EID2[i]; i++){
        UART_OutChar(setUpper(EID2[i]));
      }
      UART_OutString("\n\rConnect PB20 to DACOUT.\n\r");
      ADC_Init(ADC0,6,ADCVREF_VDDA); // PB20, 0 to 3.3V range
      ADC_Start(ADC0);
      PeriodicTask2 = lab5grader;
      TimerG7_IntArm(40000,20,1); // 100 Hz
      Time = 0;
    }
  }else{
    UART_OutString("\n\rPlease enter your two EIDs into ECE319K_Lab5main.c\n\r");
  }
}


