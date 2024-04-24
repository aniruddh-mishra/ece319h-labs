/*
 * UART.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Aniruddh Mishra
 */

#ifndef UART_H_
#define UART_H_

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"

#define PA8INDEX  18 // UART1_TX
#define PA22INDEX 46 // UART2_RX
#define FIFOSIZE 32 // maximum storage of FIFO is FIFOSIZE - 1 elements

class UART {
    int8_t shiftX;
    int8_t shiftY;
    int8_t rotation;
    bool bullet;
    bool selection;
    bool ready;
    bool win;
    bool lose;
    bool pause;


    bool shiftXRead;
    bool shiftYRead;
    bool rotationRead;
    bool bulletRead;
    bool selectionRead;
    bool readyRead;
    bool winRead;
    bool loseRead;
    bool pauseRead;

public:
    UART();

    void Out(uint8_t);
    void putShiftX(int8_t);
    void putShiftY(int8_t);
    void putRotation(int8_t);
    void putBullet(bool);
    void putSelection(bool);
    void putReady(bool);
    void putWin(bool);
    void putLose(bool);
    void putPause(bool);


    int8_t getShiftX();
    int8_t getShiftY();
    int8_t getRotation();
    bool getBullet();
    int8_t getSelection();
    bool getReady();
    bool getWin();
    bool getLose();
    bool getPause();
};

#endif /* UART_H_ */
