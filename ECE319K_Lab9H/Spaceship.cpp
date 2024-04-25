/*
 * Spaceship.cpp
 *
 *  Created on: Apr 13, 2024
 *      Author: Aniruddh Mishra
 */

#include "Spaceship.h"

#define trigDivisor 10000000

const int32_t cosQuarters[] = {1, 0, -1, 0};
const int32_t sinQuarters[] = {0, -1, 0, 1};

const uint32_t sidesThetas[] = {11, 13, 15, 15, 15, 15, 15, 15, 13};
const uint16_t* spaceShipMasterImages[] = {SpaceShip0, SpaceShip10, SpaceShip20, SpaceShip30, SpaceShip40, SpaceShip50, SpaceShip60, SpaceShip70, SpaceShip80};
const uint16_t* spaceShipSecondaryImages[] = {SpaceShip0_2, SpaceShip10_2, SpaceShip20_2, SpaceShip30_2, SpaceShip40_2, SpaceShip50_2, SpaceShip60_2, SpaceShip70_2, SpaceShip80_2};

Spaceship::Spaceship(uint32_t x_init, uint32_t y_init, bool master) {
    this->currentPos[0] = x_init;
    this->currentPos[1] = y_init;
    this->lastDrawn[0] = x_init;
    this->lastDrawn[1] = y_init;
    this->width = 11;
    this->height = 11;
    this->maxWidth = 17;
    this->maxHeight = 17;
    this->speed = 1;
    this->hp = 0;

    this->master = master;

    baseImage = SpaceShip0;
    if (!master) {
        baseImage = SpaceShip0_2;
    }
    for (int i = 0; i < width*height; i++) {
        this->bitMap[i] = baseImage[i];
    }
}

void Spaceship::Initialize(bool master) {
    this->hp = 3;
    this->master = master;
    this->degrees = 0;

    baseImage = SpaceShip0;
    if (!master) {
        baseImage = SpaceShip0_2;
    }
    for (int i = 0; i < width*height; i++) {
        this->bitMap[i] = baseImage[i];
    }
}

void Spaceship::setOrientation(uint32_t xPos, uint32_t yPos, int32_t degrees) {
    this->currentPos[0] = xPos;
    this->currentPos[1] = yPos;
    this->degrees = degrees - 10;

    rotateShip(1);
}

bool Spaceship::isMaster() {
    return master;
}

void Spaceship::moveShip(int8_t x_inc, int8_t y_inc) {
    this->lastPos[0] = this->currentPos[0];
    this->lastPos[1] = this->currentPos[1];
    int32_t newX = this->currentPos[0] - speed * x_inc;
    int32_t newY = this->currentPos[1] - speed * y_inc;
    if (newX + height >= ST7735_TFTWIDTH) this->currentPos[0] = ST7735_TFTWIDTH - height;
    else if (newX < 0) this->currentPos[0] = 0;
    else this->currentPos[0] = newX;
    if (newY + width >= ST7735_TFTHEIGHT) this->currentPos[1] = ST7735_TFTHEIGHT - width;
    else if (newY <= 0) this->currentPos[1] = 1;
    else this->currentPos[1] = newY;
}

void Spaceship::rotateQuarters(uint32_t numQuarters) {
    uint16_t newBitMap[height * width];

    for (int i = 0; i < height * width; i++) {
        newBitMap[i] = 0;
    }

    for (int32_t x = 0; x < height; x++) {
        for (int32_t y = 0; y < width; y++) {
            int32_t coord[2] = {x - (int) height / 2, (int) width / 2 - y};
            int32_t newX = coord[0] * cosQuarters[numQuarters] - coord[1] * sinQuarters[numQuarters];
            int32_t newY = coord[0] * sinQuarters[numQuarters] + coord[1] * cosQuarters[numQuarters];

            newBitMap[(newX + height / 2) * height + (width / 2 - newY)] = bitMap[x*height + y];
        }
    }

    for (int i = 0; i < height * width; i++) {
        bitMap[i] = newBitMap[i];
    }
}

bool Spaceship::fetchMap(uint32_t numRot) {
    int32_t shiftedCoord[] = {(int) currentPos[0] - ((int) sidesThetas[numRot] - (int) height) / 2, (int) currentPos[1] - ((int) sidesThetas[numRot] - (int) width) / 2};

    if (shiftedCoord[0] < 0 || shiftedCoord[1] <= 0 ||
            shiftedCoord[0] + height >= ST7735_TFTWIDTH ||
            shiftedCoord[1] + width > ST7735_TFTHEIGHT) {
        return false;
    }

    const uint16_t* image = spaceShipMasterImages[numRot];

    if (!master) {
        image = spaceShipSecondaryImages[numRot];
    }

    for (int i = 0; i < sidesThetas[numRot] * sidesThetas[numRot]; i++) {
        bitMap[i] = image[i];
    }

    this->currentPos[0] = shiftedCoord[0];
    this->currentPos[1] = shiftedCoord[1];

    height = sidesThetas[numRot];
    width = sidesThetas[numRot];

    return true;
}

void Spaceship::rotateShip(int8_t rotation) {
    int32_t oldDegrees = degrees;
    degrees = (degrees + 10 * rotation) % 360;
    if (degrees < 0) degrees += 360;
    if (!fetchMap((degrees % 90) / 10)) {
        degrees = oldDegrees;
        return;
    }

    rotateQuarters(degrees / 90);
}

uint32_t Spaceship::getCenterX() {
    return currentPos[0] + width / 2;
}

uint32_t Spaceship::getCenterY() {
    return currentPos[1] + height / 2;
}

int32_t Spaceship::getDegrees() {
    return degrees;
}

static uint16_t blackBox[289];
void Spaceship::drawShip() {
    uint32_t currentSave[2] = {currentPos[0], currentPos[1]}; // in case of interrupt
    if (lastDrawn[0] != currentSave[0] || lastDrawn[1] != currentSave[1]) {
        uint32_t boxH[2] = {lastDrawn[0], lastDrawn[1]};
        uint32_t boxV[2] = {lastDrawn[0], lastDrawn[1]};
        uint32_t xLen = currentSave[0] - lastDrawn[0];
        uint32_t yLen = currentSave[1] - lastDrawn[1];

        if (xLen > lastDrawnSideLen || yLen > lastDrawnSideLen) ST7735_DrawBitmap(lastDrawn[0], lastDrawn[1], blackBox, lastDrawnSideLen, lastDrawnSideLen);
        else {
            if (xLen > 0) {
                ST7735_DrawBitmap(boxH[0], boxH[1], blackBox, lastDrawnSideLen, xLen); // horizontal box
            }
            if (yLen > 0) {
                ST7735_DrawBitmap(boxV[0], boxV[1], blackBox, yLen, lastDrawnSideLen); // vertical box
            }
        }

        boxH[0] = currentSave[0] + height;
        boxV[1] = currentSave[1] + width;
        xLen = lastDrawn[0] + lastDrawnSideLen - currentSave[0] - height;
        yLen = lastDrawn[1] + lastDrawnSideLen - currentSave[1] - width;

        if (xLen > lastDrawnSideLen || yLen > lastDrawnSideLen) ST7735_DrawBitmap(lastDrawn[0], lastDrawn[1], blackBox, lastDrawnSideLen, lastDrawnSideLen);
        else {
            if (xLen > 0) {
                ST7735_DrawBitmap(boxH[0], boxH[1], blackBox, lastDrawnSideLen, xLen); // horizontal box
            }
            if (yLen > 0) {
                ST7735_DrawBitmap(boxV[0], boxV[1], blackBox, yLen, lastDrawnSideLen); // vertical box
            }
        }

        this->lastDrawn[0] = currentSave[0];
        this->lastDrawn[1] = currentSave[1];
        lastDrawnSideLen = height;
    }
    ST7735_DrawBitmap(currentSave[0], currentSave[1], bitMap, width, height);
}

bool Spaceship::checkBullets(Bullet* bullets, uint32_t numBullets) {
    for (int i = 0; i < numBullets; i++) {
        if (!bullets[i].exists) continue;
        if (bullets[i].getX() >= currentPos[0] && bullets[i].getX() + 3 <= currentPos[0] + width &&
                bullets[i].getY() >= currentPos[1] && bullets[i].getY() + 3 <= currentPos[1] + height) {
            if (!bullets[i].originated) continue;
            bullets[i].exists = false;
            return true;
        }

        if (!bullets[i].originated && bullets[i].source == master) bullets[i].originated = true;
    }

    return false;
}

void Spaceship::sendPosition(UART comms) {
    uint8_t x_axis_1 = currentPos[0] >> 5;
    uint8_t x_axis_2 = currentPos[0] & 0x1F;
    uint8_t y_axis_1 = currentPos[1] >> 5;
    uint8_t y_axis_2 = currentPos[1] & 0x1F;

    comms.Out(x_axis_1 & (~0xE0));
    comms.Out(x_axis_2 & (~0xE0) | 0x20);
    comms.Out(y_axis_1 & (~0xE0) | 0x40);
    comms.Out(y_axis_2 & (~0xE0) | 0x60);
}

void Spaceship::sendOrientation(UART comms) {
    comms.Out(((uint8_t) (degrees / 10)) | 0xC0);
}

void Spaceship::setFromComms(UART comms) {
    setOrientation(comms.getXAxis(), comms.getYAxis(), 10 * comms.getDegrees());

    this->hp = comms.getOtherHp();
}

void Spaceship::setComms(UART& comms) {
    comms.setXAxis(this->currentPos[0]);
    comms.setYAxis(this->currentPos[1]);
    comms.setDegrees(this->degrees / 10);
}
