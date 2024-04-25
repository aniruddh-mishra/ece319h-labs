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
    UART2->IBRD = 800;//   divider = 800 + 0/64 = 800
    UART2->FBRD = 0; // baud =2,500,000/800 = 3125
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
    UART1->IBRD = 800;//   divider = 800 + 0/64 = 800
    UART1->FBRD = 0; // baud =2,500,000/800 = 3125
    UART1->LCRH = 0x00000030;
    UART1->CTL0 |= 0x01; // enable UART1
}

UART::UART() {
    UART1_Init();
    UART2_Init();
    shiftXRead = 1;
    shiftYRead = 1;
    rotationRead = 1;
    bulletRead = 1;
    selectionRead = 1;
    readyRead = 1;
    winRead = 1;
    loseRead = 1;
}

void UART::Out(uint8_t data) {
    UART1->TXDATA = data;
}

void UART::putShiftX(int8_t shiftX) {
    if (shiftXRead) this->shiftX = shiftX;
    shiftXRead = 0;
}

void UART::putShiftY(int8_t shiftY) {
    if (shiftYRead) this->shiftY = shiftY;
    shiftYRead = 0;
}

void UART::putRotation(int8_t rotation) {
    if (rotationRead) this->rotation = rotation;
    rotationRead = 0;
}

void UART::putBullet(bool newBullet) {
    if (bulletRead) this->bullet = newBullet;
    bulletRead = 0;
}

void UART::putSelection(bool master) {
    if (selectionRead) this->selection = master;
    selectionRead = 0;
}

void UART::putReady(bool ready) {
    if (readyRead) this->ready = ready;
    readyRead = 0;
}

void UART::putWin(bool win) {
    if (winRead) this->win = win;
    winRead = 0;
}

void UART::putLose(bool lose) {
    if (loseRead) this->lose = lose;
    loseRead = 0;
}

void UART::putPause(bool pause) {
    if (pauseRead) this->pause = pause;
    pauseRead = 0;
}

uint32_t UART::getYAxis() {
    if (!shiftXRead) {
        shiftXRead = 1;
        return shiftX;
    }
    return 0;
}

uint32_t UART::getXAxis() {
    if (!shiftYRead) {
        shiftYRead = 1;
        return shiftY;
    }
    return 0;
}

int32_t UART::getDegrees() {
    if (!rotationRead) {
        rotationRead = 1;
        return rotation;
    }
    return 0;
}

uint8_t UART::getOtherHp() {
    return 0;
}

uint8_t UART::getSelfHp() {
    return 0;
}

bool UART::getBullet() {
    if (!bulletRead) {
        bulletRead = 1;
        return bullet;
    }
    return 0;
}

int8_t UART::getSelection() {
    if (!selectionRead) {
        selectionRead = 1;
        if (selection) return 0;
        return 1;
    }
    return -1;
}

bool UART::getReady() {
    if (!readyRead) {
        readyRead = 1;
        return this->ready;
    }
    return false;
}

bool UART::getWin() {
    if (!winRead) {
        winRead = 1;
        return this->win;
    }
    return false;
}

bool UART::getLose() {
    if (!loseRead) {
        loseRead = 1;
        return this->lose;
    }
    return false;
}

bool UART::getPause() {
    if (!pauseRead) {
        pauseRead = 1;
        return this->pause;
    }
    return false;
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
        if (data == 0xF1) {
            comms.putRotation(-1);
        }
        else if (data == 0xF2) {
            comms.putRotation(1);
        }
        else if (data == 0xF0) {
            comms.putBullet(true);
        }
        else if (data == 0xF3) {
            comms.putSelection(true);
        }
        else if (data == 0xF4) {
            comms.putSelection(false);
        }
        else if (data == 0xF5) {
            comms.putReady(true);
        }
        else if (data == 0xF6) {
            comms.putWin(true);
        }
        else if (data == 0xF7) {
            comms.putLose(true);
        }
        else if (data == 0xF8) {
            comms.putPause(true);
        }
        else if ((data & (~0xF)) == 0xD0) {
            data &= ~0xF0;
            if (data & (~0x7)) {
                data |= 0xF0;
            }
            comms.putShiftX((int8_t) data);
        }
        else if ((data & (~0xF)) == 0xE0) {
            data &= ~0xF0;
            if (data & (~0x7)) {
                data |= 0xF0;
            }
            comms.putShiftY((int8_t) data);
        }
    }

    // read all data, putting in FIFO
    // finish writing this
    GPIOB->DOUTTGL31_0 = BLUE; // toggle PB22 (minimally intrusive debugging)
}
