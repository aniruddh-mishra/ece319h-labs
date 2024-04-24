/*
 * Bullet.cpp
 *
 *  Created on: Apr 21, 2024
 *      Author: Aniruddh Mishra
 */

#include "Bullet.h"
#define DIVISOR 10000

const int32_t stepX[9] = {-10000, -9848, -9397, -8660, -7660, -6428, -5000, -3420, -1736};
const int32_t stepY[9] = {0, -1736, -3420, -5000, -6428, -7660, -8660, -9397, -9848};

const int32_t cosQuarters[] = {1, 0, -1, 0};
const int32_t sinQuarters[] = {0, 1, 0, -1};

void Bullet::initialize(uint32_t startX, uint32_t startY, int32_t degrees, bool sourceMaster) {
    currentPos[0] = startX - 1;
    currentPos[1] = startY - 1;
    lastDrawn[0] = startX - 1;
    lastDrawn[1] = startY - 1;
    exists = true;
    originated = false;
    source = sourceMaster;

    stepSize[0] = stepX[(degrees%90) / 10];
    stepSize[1] = stepY[(degrees%90) / 10];

    int32_t cos = cosQuarters[degrees/90];
    int32_t sin = sinQuarters[degrees/90];

    int32_t newSteps[2] = {stepSize[0] * cos - stepSize[1] * sin,
                        stepSize[1] * cos + stepSize[0] * sin};

    stepSize[0] = newSteps[0];
    stepSize[1] = newSteps[1];

    for (int i = 0; i < 9; i++) {
        bitMap[i] = BulletImage[i];
    }
}

void Bullet::stepBullet() {
    totalSteps[0] += stepSize[0];
    totalSteps[1] += stepSize[1];

    currentPos[0] += totalSteps[0] / DIVISOR * 2;
    currentPos[1] += totalSteps[1] / DIVISOR * 2;

    totalSteps[0] -= DIVISOR * (totalSteps[0] / DIVISOR);
    totalSteps[1] -= DIVISOR * (totalSteps[1] / DIVISOR);

    if (currentPos[0] + 3 >= ST7735_TFTWIDTH ||
            currentPos[0] < 0 ||
            currentPos[1] + 3 >= ST7735_TFTHEIGHT ||
            currentPos[1] <= 0)
        exists = false;
}

static uint16_t blackBox[9];

void Bullet::drawBullet() {
    ST7735_DrawBitmap(lastDrawn[0], lastDrawn[1], blackBox, 3, 3);
    lastDrawn[0] = currentPos[0];
    lastDrawn[1] = currentPos[1];
    if (exists && originated) {
        ST7735_DrawBitmap(currentPos[0], currentPos[1], bitMap, 3, 3);
    }
}

void Bullet::clear() {
    ST7735_DrawBitmap(lastDrawn[0], lastDrawn[1], blackBox, 3, 3);
}

uint32_t Bullet::getX() {
    return currentPos[0];
}

uint32_t Bullet::getY() {
    return currentPos[1];
}
