/*
 * GameState.cpp
 *
 *  Created on: Apr 26, 2024
 *      Author: Aniruddh Mishra
 */

#include "GameState.h"

GameState::GameState() {}

GameState::GameState(int stageNum, int time) {
    for (int i = 0; i < 64; i ++) {
        next[i] = this;
    }

    stage = stageNum;
    minInterrupts = time;
}

// Adds next states to next state based on binary input values to trigger change
void GameState::Initialize(uint8_t* inputs, GameState** nextStates, uint32_t numChanges) {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < numChanges; j++) {
            if ((i & inputs[j]) == inputs[j]) next[i] = nextStates[j];
        }
    }
}
