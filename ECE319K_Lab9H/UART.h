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
    bool nextStateFlag;
    bool nextStateRead;

    bool characterSelectionMaster;
    bool characterSelectionRead;

    bool bulletRead;

    uint8_t selfHp;

    uint8_t otherHp;

    uint32_t xAxis;
    uint32_t updatingXAxis;

    uint32_t yAxis;
    uint32_t updatingYAxis;

    uint32_t degrees;

public:
    UART();

    // Used by external client
    bool getNextStateFlag();
    int8_t getCharacterSelection();
    bool getBullet();
    uint8_t getSelfHp();
    uint8_t getOtherHp();
    uint32_t getXAxis();
    uint32_t getYAxis();
    uint32_t getDegrees();

    // Used by UART interrupt
    void putNextStateFlag(bool);
    void putCharacterSelection(bool);
    void putBullet();
    void putSelfHp(uint8_t);
    void putOtherHp(uint8_t);
    void putXAxis_1(uint8_t);
    void putXAxis_2(uint8_t);
    void putYAxis_1(uint8_t);
    void putYAxis_2(uint8_t);
    void putDegrees(uint8_t);

    // Comms Interaction Functions
    void Out(uint8_t);
    void setXAxis(uint32_t);
    void setYAxis(uint32_t);
    void setDegrees(uint32_t);
    void resetHp();
};

#endif /* UART_H_ */
