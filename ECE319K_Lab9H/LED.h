/*
 * LED.h
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */

#ifndef LED_H_
#define LED_H_

// initialize your LEDs
void LED_Init(void);

void LED_Array(uint32_t);

void LED_Clear();

// data specifies which LED to turn on
void LED_On(uint32_t data);

// data specifies which LED to turn off
void LED_Off(uint32_t data);

// data specifies which LED to toggle
void LED_Toggle(uint32_t data);

#endif /* LED_H_ */
