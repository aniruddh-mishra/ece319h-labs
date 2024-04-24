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

class Spaceship {
    uint32_t lastDrawn[2];
    uint32_t lastDrawnSideLen;
    uint32_t lastPos[2];
    uint32_t width;
    uint32_t height;
    uint32_t maxWidth;
    uint32_t maxHeight;
    bool master;
    uint16_t bitMap[289];
    const uint16_t* baseImage;

public:
    uint32_t currentPos[2];
    uint32_t speed;
    uint32_t hp;
    int32_t degrees;
    Spaceship(uint32_t, uint32_t, bool);
    void Initialize(bool);
    void setOrientation(uint32_t, uint32_t, int32_t);
    void moveShip(int8_t, int8_t);
    void rotateShip(int8_t);
    void rotateQuarters(uint32_t);
    void rotateMap(int32_t, int32_t);
    bool fetchMap(uint32_t);
    bool checkBullets(Bullet*, uint32_t);
    bool isMaster();
    uint32_t getX();
    uint32_t getY();
    uint32_t getCenterX();
    uint32_t getCenterY();
    void drawShip();
};

#endif /* SPACESHIP_H_ */
