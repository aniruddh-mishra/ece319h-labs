/* ECE319K_Lab4main.c
 * Traffic light FSM
 * ECE319H students must use pointers for next state
 * ECE319K students can use indices or pointers for next state
 * Aniruddh Mishra & Evan Lai
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>

#define goS FSM
#define slowS FSM + 1
#define stopS FSM + 2
#define goWlk FSM + 3
#define flashWlk1 FSM + 4
#define flashWlk2 FSM + 5
#define flashWlk3 FSM + 6
#define flashWlk4 FSM + 7
#define stopWlk FSM + 8
#define goW FSM + 9
#define slowW FSM + 10
#define stopW FSM + 11

// put both EIDs in the next two lines
const char EID1[] = "AM97437"; //  ;replace abc123 with your EID
const char EID2[] = "EXL79"; //  ;replace abc123 with your EID

typedef struct State{
    int out;
    int time;
    struct State* next[8];
} State;

// initialize 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void){ // assumes LaunchPad_Init resets and powers A and B
    uint32_t outPinCM = 0x00000081;
    uint32_t inPinCM = 0x00040081;

    // Sets modes of South Lights
    IOMUX->SECCFG.PINCM[14] = outPinCM;
    IOMUX->SECCFG.PINCM[12] = outPinCM;
    IOMUX->SECCFG.PINCM[11] = outPinCM;

    // Sets modes of West Lights
    IOMUX->SECCFG.PINCM[24] = outPinCM;
    IOMUX->SECCFG.PINCM[23] = outPinCM;
    IOMUX->SECCFG.PINCM[22] = outPinCM;

    // Sets modes of buttons (sensors)
    IOMUX->SECCFG.PINCM[42] = inPinCM;
    IOMUX->SECCFG.PINCM[32] = inPinCM;
    IOMUX->SECCFG.PINCM[31] = inPinCM;

    // Sets LED pins to output enabled
    GPIOB->DOE31_0 |= 0x0C4001C7;
}

/* Activate LEDs
* Inputs: data1,data2,data3
*   specify what these means
* Output: none
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
void Traffic_Out(uint32_t outputData){
    GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & ~(0x0C4001C7)) | outputData; // replaces pins 0-2, 6-8, 27-26, 22 with desired output
}

/* Read sensors
 * Input: none
 * Output: sensor values
*   specify what these means
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void){
    return (GPIOB->DIN31_0 & 0x00038000) >> 15; // bit arithmetic to extract input
}

// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
void Grader_Init(void);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
int main2(void){ // main2
    Clock_Init80MHz(0);
    LaunchPad_Init();
    Grader_Init();  // execute this line before your code
    Traffic_Init(); // your Lab 4 initialization
    if((GPIOB->DOE31_0 & 0x20)==0){
        UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
    }
    Debug_Init();   // Lab 3 debugging
    UART_Init();
    UART_OutString("Lab 4, Spring 2024, Step 1. Debug LEDs\n\r");
    UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
    UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
    while(1){
        //write debug code to test your Traffic_Out
        // Call Traffic_Out testing all LED patterns
        // Lab 3 dump to record output values

        Traffic_Out(0x00000001); // PB0
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x00000002); // PB1
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x00000004); // PB2
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x00000040); // PB6
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x00000080); // PB7
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x00000100); // PB8
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x04000000); // PB26
        Debug_Dump(GPIOB->DOUT31_0);
        Clock_Delay(40000000); // 0.5s

        Traffic_Out(0x0C400000); // PB27, PB26, PB22
        Debug_Dump(GPIOB->DOUT31_0);

        Clock_Delay(40000000); // 0.5s
        if((GPIOB->DOUT31_0&0x20) == 0){
          UART_OutString("DOUT not friendly\n\r");
        }
    }
}
// use main3 to debug the three input switches
int main3(void){ // main3
    uint32_t last=0,now;
    Clock_Init80MHz(0);
    LaunchPad_Init();
    Traffic_Init(); // your Lab 4 initialization
    Debug_Init();   // Lab 3 debugging
    UART_Init();
    __enable_irq(); // UART uses interrupts
    UART_OutString("Lab 4, Spring 2024, Step 2. Debug switches\n\r");
    UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
    UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
    while(1){
        now = Traffic_In(); // Your Lab4 input
        if(now != last){ // change
          UART_OutString("Switch= 0x"); UART_OutUHex(now); UART_OutString("\n\r");
          Debug_Dump(now);
        }
        last = now;
        Clock_Delay(800000); // 10ms, to debounce switch
    }
}
// use main4 to debug using your dump
// proving your machine cycles through all states
int main4(void){// main4
    uint32_t input = 7;

    Clock_Init80MHz(0);
    LaunchPad_Init();
    LaunchPad_LED1off();
    Traffic_Init(); // your Lab 4 initialization
    Debug_Init();   // Lab 3 debugging
    UART_Init();
    __enable_irq(); // UART uses interrupts
    UART_OutString("Lab 4, Spring 2024, Step 3. Debug FSM cycle\n\r");
    UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
    UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");

    // initialize your FSM
    State FSM[12] = {
        {0x04000101, 1000, {goS, slowS, goS, slowS, slowS, slowS, slowS, slowS}}, // go South
        {0x04000102, 750, {stopS, stopS, stopS, stopS, stopS, stopS, stopS, stopS}}, // slow South
        {0x04000104, 500, {goS, goW, goS, goW, goWlk, goWlk, goWlk, goWlk}}, // stop South
        {0x0C400104, 1000, {goWlk, flashWlk1, flashWlk1, flashWlk1, goWlk, flashWlk1, flashWlk1, flashWlk1}}, // go Walk
        {0x04000104, 500, {flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2}}, // flash Walk1
        {0x00000104, 500, {flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3}}, // flash Walk2
        {0x04000104, 500, {flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4}}, // flash Walk3
        {0x00000104, 500, {stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk}}, // flash Walk4
        {0x04000104, 500, {goWlk, goW, goS, goW, goWlk, goW, goS, goW}}, // stop Walk
        {0x04000044, 1000, {goW, goW, slowW, slowW, slowW, slowW, slowW, slowW}}, // go West
        {0x04000084, 750, {stopW, stopW, stopW, stopW, stopW, stopW, stopW, stopW}}, // slow West
        {0x04000104, 500, {goW, goW, goS, goS, goWlk, goWlk, goS, goS}}, // stop West
    };
    State* currentState = FSM;

    SysTick_Init();   // Initialize SysTick for software waits

    while(1){
        // 1) output depending on state using Traffic_Out
        // call your Debug_Dump logging your state number and output
        Traffic_Out(currentState->out);

        Debug_Dump(GPIOB->DOUT31_0 & ~(0x00FF0000) | (currentState - FSM) << 12);

        // 2) wait depending on state
        SysTick_Wait10ms(currentState->time / 10); // divide by 10 for ms

        // 3) hard code this so input always shows all switches pressed
        // 4) next depends on state and input
        currentState = currentState->next[input];
    }
}
// use main5 to grade
int main(void){// main5
    uint32_t input;
    Clock_Init80MHz(0);
    LaunchPad_Init();
    Grader_Init();  // execute this line before your code
    Traffic_Init(); // your Lab 4 initialization
    SysTick_Init(); // Initialize SysTick for software waits
    Debug_Init();

    // initialize your FSM
    State FSM[12] = {
        {0x04000101, 1000, {goS, slowS, goS, slowS, slowS, slowS, slowS, slowS}}, // go South
        {0x04000102, 750, {stopS, stopS, stopS, stopS, stopS, stopS, stopS, stopS}}, // slow South
        {0x04000104, 500, {goS, goW, goS, goW, goWlk, goWlk, goWlk, goWlk}}, // stop South
        {0x0C400104, 1000, {goWlk, flashWlk1, flashWlk1, flashWlk1, goWlk, flashWlk1, flashWlk1, flashWlk1}}, // go Walk
        {0x04000104, 500, {flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2, flashWlk2}}, // flash Walk1
        {0x00000104, 500, {flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3, flashWlk3}}, // flash Walk2
        {0x04000104, 500, {flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4, flashWlk4}}, // flash Walk3
        {0x00000104, 500, {stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk, stopWlk}}, // flash Walk4
        {0x04000104, 500, {goWlk, goW, goS, goW, goWlk, goW, goS, goW}}, // stop Walk
        {0x04000044, 1000, {goW, goW, slowW, slowW, slowW, slowW, slowW, slowW}}, // go West
        {0x04000084, 750, {stopW, stopW, stopW, stopW, stopW, stopW, stopW, stopW}}, // slow West
        {0x04000104, 500, {goW, goW, goS, goS, goWlk, goWlk, goS, goS}}, // stop West
    };
    State* currentState = FSM;

    Lab4Grader(1); // activate UART, grader and interrupts
    while(1){
        // 1) output depending on state using Traffic_Out
        // call your Debug_Dump logging your state number and output
        Traffic_Out(currentState->out);

        Debug_Dump(GPIOB->DOUT31_0 & ~(0x00FF0000) | (currentState - FSM) << 16);

        // 2) wait depending on state
        SysTick_Wait10ms(currentState->time / 10); // divide by 10 for ms

        // 3) input from switches
        input = Traffic_In();

        // 4) next depends on state and input
        currentState = currentState->next[input];
    }
}

