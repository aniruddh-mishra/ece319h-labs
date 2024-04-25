// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Aniruddh Mishra
// Last Modified: 04/12/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "Sensors.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "images/images.h"
#include "UART.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);

// define LEDs
#define RED_LED 0
#define YELLOW_LED 1
#define GREEN_LED 2

const uint32_t LEDS[3] = {RED_LED, YELLOW_LED, GREEN_LED};

// define Switches
#define BUTTON_UP 8
#define BUTTON_LEFT 4
#define BUTTON_DOWN 2
#define BUTTON_RIGHT 1
#define NUM_BULLETS 100

bool updating = false;
int8_t lang = -1;

uint8_t updates;
Sensors inputs;
Spaceship playerShip(50, 70, false);
Spaceship opponentShip(69, 85, false);

Bullet bullets[NUM_BULLETS];
uint32_t numBullets;

UART comms;

#define langState FSM
#define mainMenu FSM + 1
#define chooseCharacter FSM + 2
#define gameStart FSM + 3 // Includes instructions (lore)
#define gameEngine FSM + 4
#define pauseState FSM + 5
#define winState FSM + 6
#define loseState FSM + 7

struct GameState {
    int stage;
    int minInterrupts;
    struct GameState* next[16];

    GameState() {}

    GameState(int stageNum, int time) {
        for (int i = 0; i < 16; i ++) {
            next[i] = this;
        }

        stage = stageNum;
        minInterrupts = time;
    }
};

/*
 * Inputs: Binary String
 *  b5: Player Alive
 *  b4: Opponent Alive
 *  b3: Down Button
 *  b2: Language Flag != -1
 *  b1: Any Button Pressed
 *  b0: Next State UART Flag
 */

GameState FSM[12] = {
    {0, 1}, // language selection
    {1, 20}, // main menu
    {2, 20}, // choose character screen
    {3, 1}, // game start + instructions state
    {4, 20}, // game play state
    {5, 60}, // pause menu
    {6, 60}, // win state
    {7, 20} // loss state
};

GameState* currentGameState = gameStart;

void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t Random32(void){
  static uint32_t M = 1;
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

void cleanBulletsArray() {
    Bullet newBullets[NUM_BULLETS];
    int j = 0;
    for (int i = 0; i < numBullets; i++) {
        if (bullets[i].exists) {
            newBullets[j] = bullets[i];
            j++;
        }
        else {
            bullets[i].clear();
        }
    }

    numBullets = j;

    for (int i = 0; i < numBullets; i++) {
        bullets[i] = newBullets[i];
    }
}

void langRun() {
    ST7735_FillScreen(ST7735_BLACK);

    ST7735_SetCursor(3, 1);
    ST7735_OutString((char *) "Choose Your", 2, ST7735_ORANGE);
    ST7735_SetCursor(5, 3);
    ST7735_OutString((char*) "Language!", 2, ST7735_ORANGE);

    ST7735_SetCursor(4, 6);
    ST7735_OutString((char*) "English", 1, ST7735_ORANGE);

    ST7735_SetCursor(16, 6);
    ST7735_OutString((char*) "Espa\xA4ol", 1, ST7735_ORANGE);
    ST7735_SetCursor(18, 8);
    ST7735_OutString((char*) "Bot\xA2n", 1, ST7735_WHITE);
    ST7735_SetCursor(16, 9);
    ST7735_OutString((char*) "Derecho", 1, ST7735_WHITE);

    ST7735_SetCursor(4, 8);
    ST7735_OutString((char*) "Left", 1, ST7735_WHITE);
    ST7735_SetCursor(4, 9);
    ST7735_OutString((char*) "Button", 1, ST7735_WHITE);
    while(currentGameState->stage == 0) {}
}

void mainMenuRun() {
    ST7735_FillScreen(ST7735_BLACK);

    Spaceship ship1(100, 50, true);
    Spaceship ship2(85, 20, false);

    ship1.setOrientation(100, 50, 50);
    ship2.setOrientation(85, 20, 200);

    ship1.rotateShip(1);
    ship2.rotateShip(-1);

    while(currentGameState->stage == 1) {
        cleanBulletsArray();
        for (int i = 0; i < Random(5); i++) {
            if (numBullets == NUM_BULLETS - 1) continue;
            bullets[numBullets].initialize(Random(ST7735_TFTWIDTH), Random(ST7735_TFTHEIGHT), Random(360), false);
            bullets[numBullets].originated = true;
            numBullets ++;
        }

        for (int i = 0; i < numBullets; i++) {
          bullets[i].drawBullet();
        }

        for (int i = 0; i < numBullets; i ++) {
            if (!Random(7)) {
                bullets[i].exists = false;
            }
        }

        ship1.drawShip();
        ship2.drawShip();

        if (lang == 1) {
            ST7735_SetCursor(2, 2);
            ST7735_OutString((char *) "It's time to ...", 1, ST7735_ORANGE);

            ST7735_SetCursor(2, 4);
            ST7735_OutString((char*) "BIT BATTLE", 2, ST7735_ORANGE);

            ST7735_SetCursor(14, 9);
            ST7735_OutString((char*) "By: Aniruddh", 1, ST7735_WHITE);
            ST7735_SetCursor(18, 10);
            ST7735_OutString((char*) "Mishra &", 1, ST7735_WHITE);
            ST7735_SetCursor(18, 11);
            ST7735_OutString((char*) "Evan Lai", 1, ST7735_WHITE);
        }
        if (!lang) {
            ST7735_SetCursor(2, 2);
            ST7735_OutString((char *) "Es tiempo de ...", 1, ST7735_ORANGE);

            ST7735_SetCursor(2, 4);
            ST7735_OutString((char*) "BATALLA DE", 2, ST7735_ORANGE);
            ST7735_SetCursor(2, 6);
            ST7735_OutString((char*) "BITS", 2, ST7735_ORANGE);

            ST7735_SetCursor(14, 9);
            ST7735_OutString((char*) "By: Aniruddh", 1, ST7735_WHITE);
            ST7735_SetCursor(18, 10);
            ST7735_OutString((char*) "Mishra &", 1, ST7735_WHITE);
            ST7735_SetCursor(18, 11);
            ST7735_OutString((char*) "Evan Lai", 1, ST7735_WHITE);
        }

        Clock_Delay(30000000);
    }

    numBullets = 0;
}

void chooseCharacterRun() {
    comms.Out(0xF5);
    playerShip.hp = 0;
    opponentShip.hp = 0;

    ST7735_FillScreen(ST7735_BLACK);

    Spaceship ship1(60, 38, true);
    Spaceship ship2(60, 110, false);

    ship1.drawShip();
    ship2.drawShip();
    if (lang == 1) {
        ST7735_SetCursor(3, 1);
        ST7735_OutString((char *) "Choose Your", 2, ST7735_ORANGE);
        ST7735_SetCursor(4, 3);
        ST7735_OutString((char*) "Character!", 2, ST7735_ORANGE);

        ST7735_SetCursor(4, 8);
        ST7735_OutString((char*) "MegaBit", 1, ST7735_ORANGE);

        ST7735_SetCursor(16, 8);
        ST7735_OutString((char*) "GigaBit", 1, ST7735_ORANGE);

        ST7735_SetCursor(18, 10);
        ST7735_OutString((char*) "Right", 1, ST7735_WHITE);
        ST7735_SetCursor(17, 11);
        ST7735_OutString((char*) "Button", 1, ST7735_WHITE);

        ST7735_SetCursor(4, 10);
        ST7735_OutString((char*) "Left", 1, ST7735_WHITE);
        ST7735_SetCursor(4, 11);
        ST7735_OutString((char*) "Button", 1, ST7735_WHITE);
    }
    if (!lang) {
        ST7735_SetCursor(4, 1);
        ST7735_OutString((char *) "Escoge tu", 2, ST7735_ORANGE);
        ST7735_SetCursor(3, 3);
        ST7735_OutString((char*) "\xADPersonaje!", 2, ST7735_ORANGE);

        ST7735_SetCursor(4, 8);
        ST7735_OutString((char*) "MegaBit", 1, ST7735_ORANGE);

        ST7735_SetCursor(16, 8);
        ST7735_OutString((char*) "GigaBit", 1, ST7735_ORANGE);

        ST7735_SetCursor(18, 10);
        ST7735_OutString((char*) "Bot\xA2n", 1, ST7735_WHITE);
        ST7735_SetCursor(16, 11);
        ST7735_OutString((char*) "Derecho", 1, ST7735_WHITE);

        ST7735_SetCursor(4, 10);
        ST7735_OutString((char*) "Bot\xA2n", 1, ST7735_WHITE);
        ST7735_SetCursor(4, 11);
        ST7735_OutString((char*) "izquierdo", 1, ST7735_WHITE);
    }
    while(currentGameState->stage == 2) {}
}

void gamePlayAnimation() {
    ST7735_FillScreen(ST7735_BLACK);

    ST7735_SetCursor(6, 1);
    ST7735_OutString((char *) "You Are", 2, ST7735_ORANGE);
    ST7735_SetCursor(6, 3);
    if (playerShip.isMaster()) {
        ST7735_OutString((char*) "Megabit", 2, ST7735_ORANGE);
    }
    else {
        ST7735_OutString((char*) "Gigabit", 2, ST7735_ORANGE);
    }

    Spaceship shipDisplay(80, 73, playerShip.isMaster());
    shipDisplay.drawShip();

    Clock_Delay(80000000);

    if (ANIRUDDH_SCREEN) {
        ST7735_FillScreen(ST7735_BLACK);
    }
    else {
        ST7735_FillScreen(~ST7735_BLACK);
    }

    ST7735_SetCursor(12, 4);
    ST7735_OutString((char *) "3", 5, ST7735_ORANGE);
    LED_On(RED_LED);

    Clock_Delay(80000000);

    ST7735_SetCursor(12, 4);
    ST7735_OutString((char *) "2", 5, ST7735_ORANGE);
    LED_On(YELLOW_LED);

    Clock_Delay(80000000);

    ST7735_SetCursor(12, 4);
    ST7735_OutString((char *) "1", 5, ST7735_ORANGE);
    LED_On(GREEN_LED);

    if (playerShip.isMaster()) {
        playerShip.setOrientation(20, 30, 60);
        opponentShip.setOrientation(90, 120, 240);
    }
    else {
        playerShip.setOrientation(90, 120, 240);
        opponentShip.setOrientation(20, 30, 60);
    }


    while (currentGameState->stage == 3) {}
}

void winRun() {
    comms.Out(0xF6);
    if (ANIRUDDH_SCREEN) {
      ST7735_FillScreen(ST7735_BLACK);
    }
    else {
      ST7735_FillScreen(~ST7735_BLACK);
    }

    if (lang == 1) {
        ST7735_SetCursor(3, 2);
        ST7735_OutString((char *) "YOU WON", 3, ST7735_GREEN);
        ST7735_SetCursor(5, 5);
        ST7735_OutString((char*) "bit battle beast", 1, ST7735_GREEN);
        ST7735_SetCursor(5, 8);
        ST7735_OutString((char*) "PRESS ANY BUTTON", 1, ST7735_WHITE);
        ST7735_SetCursor(7, 9);
        ST7735_OutString((char*) "TO CONTINUE", 1, ST7735_WHITE);
    }
    if (!lang) {
        ST7735_SetCursor(3, 2);
        ST7735_OutString((char *) "GANASTE", 3, ST7735_GREEN);
        ST7735_SetCursor(1, 5);
        ST7735_OutString((char*) "peque\xA4""a bestia de batalla", 1, ST7735_GREEN);
        ST7735_SetCursor(5, 8);
        ST7735_OutString((char*) "PRESS ANY BUTTON", 1, ST7735_WHITE);
        ST7735_SetCursor(7, 9);
        ST7735_OutString((char*) "TO CONTINUE", 1, ST7735_WHITE);
    }
    while (currentGameState->stage == 5) {};
}

void loseRun() {
    comms.Out(0xF7);
    if (ANIRUDDH_SCREEN) {
      ST7735_FillScreen(ST7735_BLACK);
    }
    else {
      ST7735_FillScreen(~ST7735_BLACK);
    }
    if (lang == 1) {
        ST7735_SetCursor(2, 2);
        ST7735_OutString((char *) "YOU LOSE", 3, ST7735_RED);
        ST7735_SetCursor(6, 5);
        ST7735_OutString((char*) "take the L lol", 1, ST7735_RED);
        ST7735_SetCursor(5, 8);
        ST7735_OutString((char*) "PRESS ANY BUTTON", 1, ST7735_WHITE);
        ST7735_SetCursor(7, 9);
        ST7735_OutString((char*) "TO CONTINUE", 1, ST7735_WHITE);
    }
    if (!lang) {
        ST7735_SetCursor(3, 2);
        ST7735_OutString((char *) "T\xA3 PIERDES", 2, ST7735_RED);
        ST7735_SetCursor(5, 4);
        ST7735_OutString((char*) "toma la L jajaja", 1, ST7735_RED);
        ST7735_SetCursor(2, 8);
        ST7735_OutString((char*) "PRESIONE CUALQUIER BOT\xA2N", 1, ST7735_WHITE);
        ST7735_SetCursor(7, 9);
        ST7735_OutString((char*) "PARA CONTINUAR", 1, ST7735_WHITE);
    }
    while (currentGameState->stage == 6) {};
}

void pauseRun() {
    int bufferHP = opponentShip.hp;
    opponentShip.hp = 0;

    if (lang == 1) {
        ST7735_SetCursor(3, 4);
        ST7735_OutString((char*) "GAME PAUSED", 2, ST7735_ORANGE);
        ST7735_SetCursor(3, 8);
        ST7735_OutString((char*) "PRESS SHOOT TO RESUME", 1, ST7735_WHITE);
    }
    if (!lang) {
        ST7735_SetCursor(1, 4);
        ST7735_OutString((char*) "JUEGO PAUSADO", 2, ST7735_ORANGE);
        ST7735_SetCursor(4, 8);
        ST7735_OutString((char*) "PULSE DISPARO PARA", 1, ST7735_WHITE);
        ST7735_SetCursor(9, 9);
        ST7735_OutString((char*) "REANUDAR", 1, ST7735_WHITE);
    }
    while (currentGameState->stage == 4) {}
    opponentShip.hp = bufferHP;
    comms.Out(0xF8);
}

void gameEngineRun() {
    inputs.InitializePosition(); // TODO: Indicate this in the instruction menu
    ST7735_FillScreen(ST7735_BLACK);

    while(currentGameState->stage == 3){
        if (updating) continue;
        playerShip.drawShip();
        opponentShip.drawShip();
        for (int i = 0; i < numBullets; i++) {
          bullets[i].drawBullet();
        }

        uint32_t LEDValue = 0;
        for (int i = 0; i < playerShip.hp; i++) {
          LEDValue += 1 << LEDS[i];
        }

        LED_Array(LEDValue);

        updating = true;
    }
}

// games  engine runs at 60Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
    if((TIMG12->CPU_INT.IIDX) == 1) {
        GPIOB->DOUTTGL31_0 = GREEN;
        inputs.In();
        Switch_In();
        GPIOB->DOUTTGL31_0 = GREEN;

        static int FSM_Counter = 0;

        FSM_Counter ++;

        if (currentGameState->minInterrupts <= FSM_Counter) return;

        if (currentGameState->stage == 3) { // game engine
            if (comms.getPause()) {
                currentGameState = pauseState;
            }
            if (Switch_Active(BUTTON_DOWN)) {
                comms.Out(0xF8);
            }
            if (comms.getLose()) {
                currentGameState = winState;
            }
            if (comms.getWin()) {
                currentGameState = loseState;
            }
        }

        uint32_t FSM_Input = Switch_FSM_Val();
        FSM_Input |= ((playerShip.hp != 0) << 2) | ((opponentShip.hp != 0) << 3);

        if (currentGameState != currentGameState->next[FSM_Input]) {
            FSM_Counter = 0;
            currentGameState = currentGameState->next[FSM_Input];
            return;
        }

        if (currentGameState->stage == 4) { // pause screen
            if (Switch_Active(BUTTON_UP)) {
                opponentShip.hp = 3;
            }

            if (comms.getPause()) {
                currentGameState = gameEngine;
            }
        }

        if (currentGameState->stage == 7) { // language select screen
            if (Switch_Active(BUTTON_RIGHT)) {
                lang = 0;
                opponentShip.hp = 3;
            }
            else if (Switch_Active(BUTTON_LEFT)) {
                lang = 1;
                opponentShip.hp = 3;
            }
        }

        static bool othersHasLanded = false;

        if (currentGameState->stage == 1) {
            othersHasLanded |= comms.getReady();
            if (!othersHasLanded) return;
            if (Switch_Active(BUTTON_RIGHT)) {
                playerShip.Initialize(false);
                opponentShip.Initialize(true);
                comms.Out(0xF3);
                othersHasLanded = false;
            }
            else if (Switch_Active(BUTTON_LEFT)) {
                playerShip.Initialize(true);
                opponentShip.Initialize(false);
                comms.Out(0xF4);
                othersHasLanded = false;
            }
            int8_t opponentSelection = comms.getSelection();
            if (opponentSelection == 1) {
                opponentShip.Initialize(true);
                playerShip.Initialize(false);
                othersHasLanded = false;
            }
            else if (opponentSelection == 0) {
                opponentShip.Initialize(false);
                playerShip.Initialize(true);
                othersHasLanded = false;
            }
        }

        if (!updating || currentGameState->stage != 3) return;

//        uint32_t pot_d = inputs.PotVal();

        static uint32_t rotCounter = 0;
        rotCounter = (rotCounter + 1) % 2;

        if (Switch_Active(BUTTON_RIGHT) && !rotCounter) {
            playerShip.rotateShip(-1);
            comms.Out(0xF1);
        }
        else if (Switch_Active(BUTTON_LEFT) && !rotCounter) {
            playerShip.rotateShip(1);
            comms.Out(0xF2);
        }

        for (int i = 0; i < numBullets; i++) {
            bullets[i].stepBullet();
        }

        // Handle accelerometer player movements and send on UART
        int8_t shiftX;
        int8_t shiftY;
        inputs.getShifts(shiftX, shiftY);
        playerShip.moveShip(shiftX, shiftY);
        playerShip.sendPosition(comms);

        // Bullet Shooting
        cleanBulletsArray();
        static bool lastShoot = 0;
        static uint32_t reloadTime = 0;
        static uint8_t ammo = 3;

        if (Switch_Active(BUTTON_UP) && !lastShoot && numBullets < NUM_BULLETS) {
            lastShoot = 1;
            if (ammo) {
                bullets[numBullets].initialize(playerShip.getCenterX(), playerShip.getCenterY(), playerShip.getDegrees(), playerShip.isMaster());
                numBullets ++;
                ammo --;
                comms.Out(0x80);
            }
            else {
                reloadTime = (reloadTime + 1) % 120;
                // TODO Start empty sound
                if (!reloadTime) ammo = 3;
            }
        }
        else if (!Switch_Active(BUTTON_UP)) lastShoot = 0;

        // Handle opponent ship movement
        opponentShip.setFromComms(comms);
        playerShip.hp = comms.getSelfHp();

        // Handle opponent ship shooting
        if (comms.getBullet() && numBullets < NUM_BULLETS) {
            bullets[numBullets].initialize(opponentShip.getCenterX(), opponentShip.getCenterY(), opponentShip.getDegrees(), opponentShip.isMaster());
            numBullets ++;
        }

        if (playerShip.checkBullets(bullets, numBullets) && playerShip.hp > 0) {
            playerShip.hp -= 1;
            comms.Out(playerShip.hp | 0x90);
        }
        if (opponentShip.checkBullets(bullets, numBullets) && opponentShip.hp > 0) {
            opponentShip.hp -=1;
            comms.Out(playerShip.hp | 0x90);
        }

        updating = false;

        GPIOB->DOUTTGL31_0 = GREEN;
    }
}

// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
//  ST7735_SetRotation(0);
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
//  Sound_Init();  // initialize sound
    // initialize interrupts on TimerG12 at 60 Hz
//  TimerG12_IntArm(80000000/60, 2);
  __enable_irq();

  while (1) {
    if (currentGameState->stage == 0) langRun();

    if (currentGameState->stage == 1) mainMenuRun();

    if (currentGameState->stage == 2) chooseCharacterRun();

    if (currentGameState->stage == 3) gamePlayAnimation();

    if (currentGameState->stage == 4) gameEngineRun();

    if (currentGameState->stage == 5) pauseRun();

    if (currentGameState->stage == 6) winRun();

    if (currentGameState->stage == 7) loseRun();
  }
}
