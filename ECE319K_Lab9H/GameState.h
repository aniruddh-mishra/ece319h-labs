/*
 * GameState.h
 *
 *  Created on: Apr 26, 2024
 *      Author: Aniruddh Mishra
 */

#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include <stdint.h>

struct GameState {
    int stage;
    int minInterrupts;
    struct GameState* next[64];

    GameState();
    GameState(int, int);
    void Initialize(uint8_t*, GameState**, uint32_t);
};

#endif /* GAMESTATE_H_ */
