/*
 * Switch.h
 *
 *  Created on: Nov 5, 2023
 *      Author: jonat
 */

#ifndef SWITCH_H_
#define SWITCH_H_

// initialize your switches
void Switch_Init(void);

// return current state of switches
void Switch_In(void);

// returns whether input switch is clicked
bool Switch_Active(uint8_t);

uint32_t Switch_FSM_Val();

#endif /* SWITCH_H_ */
