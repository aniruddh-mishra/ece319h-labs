/*
 * Spaceship.h
 *
 *  Created on: Apr 13, 2024
 *      Author: Aniruddh Mishra
 */

#ifndef BULLET_H_
#define BULLET_H_

#include <stdint.h>
#include "images/images.h"
#include "../inc/ST7735.h"

class Bullet {
    int32_t totalSteps[2];
    int32_t currentPos[2];
    int32_t lastDrawn[2];
    int32_t stepSize[2];
    uint16_t bitMap[9];

public:
    bool exists;
    bool originated;
    bool source;
    void initialize(uint32_t, uint32_t, int32_t, bool);
    void stepBullet();
    void drawBullet();
    void clear();
    uint32_t getX();
    uint32_t getY();
};

#endif /* BULLET_H_ */
