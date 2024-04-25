/*
 * UART.cpp
 *
 *  Created on: Apr 22, 2024
 *      Author: Aniruddh Mishra
 */

#include "UART.h"

void UART2_Init() {
    // UART2 Initialization
    UART2->GPRCM.RSTCTL = 0xB1000003;
    UART2->GPRCM.PWREN = 0x26000001;
    Clock_Delay(24); // time for uart to power up
    IOMUX->SECCFG.PINCM[PA22INDEX]  = 0x00040082;
    UART2->CLKSEL = 0x08; // bus clock
    UART2->CLKDIV = 0x00; // no divide
    UART2->CTL0 &= ~0x01; // disable UART2
    UART2->CTL0 = 0x00020018;
    // assumes an 80 MHz bus clock
    UART2->IBRD = 400;//   divider = 100 + 0/64 = 100
    UART2->FBRD = 0; // baud =2,500,000/100 = 3125
    UART2->LCRH = 0x00000030;
    UART2->CPU_INT.IMASK = 1;
    // bit 11 TXINT
    // bit 10 RXINT             Interrupt only on receiver timeout RTOUT (no TXINT, no RXINT)
    // bit 0  Receive timeout
    UART2->IFLS = 0x0422;
    // bits 11-8 RXTOSEL receiver timeout select 4 (0xF highest)
    // bits 6-4  RXIFLSEL 2 is greater than or equal to half
    // bits 2-0  TXIFLSEL 2 is less than or equal to half
    NVIC->ICPR[0] = 1<<14; // UART2 is IRQ 14
    NVIC->ISER[0] = 1<<14;
    NVIC->IP[3] = (NVIC->IP[3]&(~0xFF000000))|(2<<22);    // priority (bits 23,22)
    UART2->CTL0 |= 0x01; // enable UART2
}

void UART1_Init() {
    // UART1 Initialization
    UART1->GPRCM.RSTCTL = 0xB1000003;
    UART1->GPRCM.PWREN = 0x26000001;
    Clock_Delay(24); // time for uart to power up
    IOMUX->SECCFG.PINCM[PA8INDEX]  = 0x00000082;
    UART1->CLKSEL = 0x08; // bus clock
    UART1->CLKDIV = 0x00; // no divide
    UART1->CTL0 &= ~0x01; // disable UART0
    UART1->CTL0 = 0x00020018;
    // assumes an 80 MHz bus clock
    UART1->IBRD = 400;//   divider = 800 + 0/64 = 800
    UART1->FBRD = 0; // baud =2,500,000/800 = 3125
    UART1->LCRH = 0x00000030;
    UART1->CTL0 |= 0x01; // enable UART1
}

UART::UART() {
    UART1_Init();
    UART2_Init();

    nextStateRead = true;
    characterSelectionRead = true;
    bulletRead = true;
    otherHp = 3;
    selfHp = 3;
}

void UART::Out(uint8_t data) {
    while((UART1->STAT&0x40) != 0x40){};

    UART1->TXDATA = data;
}

bool UART::getNextStateFlag() {
    if (nextStateRead) return false;

    nextStateRead = true;
    return nextStateFlag;
}

int8_t UART::getCharacterSelection() {
    if (characterSelectionRead) return -1;

    characterSelectionRead = true;
    return characterSelectionMaster;
}

bool UART::getBullet() {
    if (bulletRead) return false;

    bulletRead = true;
    return true;
}

uint8_t UART::getSelfHp() {
    return selfHp;
}

uint8_t UART::getOtherHp() {
    return otherHp;
}

uint32_t UART::getXAxis() {
    return xAxis;
}

uint32_t UART::getYAxis() {
    return yAxis;
}

uint32_t UART::getDegrees() {
    return degrees;
}

void UART::putNextStateFlag(bool flag) {
    if (!nextStateRead) return;

    nextStateRead = false;
    nextStateFlag = flag;
}

void UART::putCharacterSelection(bool master) {
    if (!characterSelectionRead) return;

    characterSelectionRead = false;
    characterSelectionMaster = master;
}

void UART::putBullet() {
    if (!bulletRead) return;

    bulletRead = false;
}

void UART::putSelfHp(uint8_t newHp) {
    selfHp = newHp;
}

void UART::putOtherHp(uint8_t newHp) {
    otherHp = newHp;
}

void UART::putXAxis_1(uint8_t xAxis) {
    updatingXAxis = 0 | (xAxis << 5);
}

void UART::putXAxis_2(uint8_t xAxis) {
    updatingXAxis |= xAxis;
    this->xAxis = updatingXAxis;
}

void UART::putYAxis_1(uint8_t yAxis) {
    updatingYAxis = 0 | yAxis << 5;
}

void UART::putYAxis_2(uint8_t yAxis) {
    updatingYAxis |= yAxis;
    this->yAxis = updatingYAxis;
}

void UART::putDegrees(uint8_t degrees) {
    this->degrees = degrees;
}

void UART::setXAxis(uint32_t xAxis) {
    this->xAxis = xAxis;
}

void UART::setYAxis(uint32_t yAxis) {
    this->yAxis = yAxis;
}

void UART::setDegrees(uint32_t degrees) {
    this->degrees = degrees;
}

void UART::resetHp() {
    otherHp = 3;
    selfHp = 3;
}

extern "C" void UART2_IRQHandler(void);
extern UART comms;

void UART2_IRQHandler() {
    uint32_t status = UART2->CPU_INT.IIDX;
    if (status != 0x01) return;

    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)

    while (!((UART2->STAT / 4) % 2)) {
        uint8_t data = UART2->RXDATA;

        if (!(data & (~0x1F))) comms.putXAxis_1(data);
        else if ((data & (~0x1F)) == 0x20) comms.putXAxis_2(data & (~0x20));
        else if ((data & (~0x1F)) == 0x40) comms.putYAxis_1(data & (~0x40));
        else if ((data & (~0x1F)) == 0x60) comms.putYAxis_2(data & (~0x60));
        else if ((data & (~0x3F)) == 0xC0) comms.putDegrees(data & (~0xC0));
        else if ((data & (~0x1F)) == 0x80) {
            if ((data & (~0x7)) == 0x80) {
                if (data == 0x80) comms.putBullet();
                if ((data & (~0x1)) == 0x82) comms.putCharacterSelection(data & (~0x82));
            }
            else if ((data & (~0x7)) == 0x88) {
                // Can add more flags but only using nextState flag
                comms.putNextStateFlag(true);
            }
            else if ((data & (~0x7)) == 0x90) {
                if ((data & (~0x3)) == 0x90) comms.putOtherHp(data & (~0x90));
                else if ((data & (~0x3)) == 0x94) comms.putSelfHp(data & (~0x94));
            }
        }
    }

    // read all data, putting in FIFO
    // finish writing this
    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)
}
