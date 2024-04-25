/*
 * Spaceship.h
 *
 *  Created on: Apr 13, 2024
 *      Author: Aniruddh Mishra
 */

#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <stdint.h>
#include "images/images.h"
#include "../inc/ST7735.h"
#include "Bullet.h"
#include "UART.h"

class Spaceship {
    uint32_t currentPos[2];
    uint32_t lastDrawn[2];
    uint32_t lastDrawnSideLen;
    uint32_t lastPos[2];
    int32_t degrees;
    uint32_t width;
    uint32_t height;
    uint32_t maxWidth;
    uint32_t maxHeight;
    uint32_t speed;
    bool master;
    uint16_t bitMap[289];
    const uint16_t* baseImage;

public:
    uint32_t hp;
    Spaceship(uint32_t, uint32_t, bool);
    void Initialize(bool);
    void setOrientation(uint32_t, uint32_t, int32_t);
    void moveShip(int8_t, int8_t);
    void rotateShip(int8_t);
    void rotateQuarters(uint32_t);
    bool fetchMap(uint32_t);
    bool checkBullets(Bullet*, uint32_t);
    bool isMaster();
    void sendPosition(UART);
    uint32_t getCenterX();
    uint32_t getCenterY();
    int32_t getDegrees();
    void setFromComms(UART);
    void drawShip();
};

#endif /* SPACESHIP_H_ */
