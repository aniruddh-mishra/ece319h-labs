/*
 * Sensors.h
 *
 *  Created on: Apr 13, 2024
 *      Author: Aniruddh Mishra
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>

class Sensors {
    // base values at initialization
    uint32_t x_base;
    uint32_t y_base;

    // current values
    uint32_t x_axis;
    uint32_t y_axis;
    uint32_t pot_d;

public:
    Sensors();
    void InitializePosition();
    void In();
    uint32_t PotVal();
    void Orientation(int32_t&, int32_t&);
};

#endif /* SENSORS_H_ */
