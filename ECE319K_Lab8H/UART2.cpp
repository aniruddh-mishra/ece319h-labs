/* UART2.cpp
 * Aniruddh Mishra
 * Data: 04/11/2024
 * PA22 UART2 Rx from other microcontroller PA8 IR output<br>
 */


#include <ti/devices/msp/msp.h>
#include "UART2.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/FIFO2.h"

uint32_t LostData;
uint32_t RXCounter;
Queue FIFO2;

// power Domain PD0
// for 80MHz bus clock, UART2 clock is ULPCLK 40MHz
// initialize UART2 for 2375 baud rate
// no transmit, interrupt on receive timeout
void UART2_Init(void){
    UART2->GPRCM.RSTCTL = 0xB1000003;
    UART2->GPRCM.PWREN = 0x26000001;
    Clock_Delay(24); // time for uart to power up
    IOMUX->SECCFG.PINCM[PA22INDEX]  = 0x00040082;
    UART2->CLKSEL = 0x08; // bus clock
    UART2->CLKDIV = 0x00; // no divide
    UART2->CTL0 &= ~0x01; // disable UART2
    UART2->CTL0 = 0x00020018;
    // assumes an 80 MHz bus clock
    UART2->IBRD = 1052;//   divider = 1052+40/64 = 1052.631579
    UART2->FBRD = 40; // baud =2,500,000/1052.631579 = 2375
    UART2->LCRH = 0x00000030;
    UART2->CPU_INT.IMASK = 0x01;
    // bit 11 TXINT
    // bit 10 RXINT
    // bit 0  Receive timeout
    UART2->IFLS = 0x0422;
    // bits 11-8 RXTOSEL receiver timeout select 4 (0xF highest)
    // bits 6-4  RXIFLSEL 2 is greater than or equal to half
    // bits 2-0  TXIFLSEL 2 is less than or equal to half
    NVIC->ICPR[0] = 1<<14; // UART2 is IRQ 15
    NVIC->ISER[0] = 1<<14;
    NVIC->IP[3] = (NVIC->IP[3]&(~0xFF000000))|(2<<22);    // priority (bits 23, 22)
    UART2->CTL0 |= 0x09; // enable UART2
}
//------------UART2_InChar------------
// Get new serial port receive data from FIFO2
// Input: none
// Output: Return 0 if the FIFO2 is empty
//         Return nonzero data from the FIFO1 if available
char UART2_InChar(void){
    char out;
    if(!FIFO2.Get(&out)) return 0;
    return out;
}

extern "C" void UART2_IRQHandler(void);
void UART2_IRQHandler(void){ 
// acknowledge, clear RTOUT
    uint32_t status = UART2->CPU_INT.IIDX;
    if (status != 0x01) return;

    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)

    while (!((UART2->STAT / 4) % 2)) {
        if (FIFO2.Put(UART2->RXDATA)) RXCounter ++;
        else LostData ++;
    }
// read all data, putting in FIFO
// finish writing this
    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)
}
