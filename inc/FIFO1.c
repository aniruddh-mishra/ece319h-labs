// FIFO1.c
// Runs on any microcontroller
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 10/29/2023
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
#include <stdint.h>

// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define SIZE 15
int8_t Put;
int8_t Get;
char FIFO[SIZE];

int8_t FULL;
int8_t EMPTY;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void Fifo1_Init(void){ //Complete this
    Put = Get = 0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is FULL
//int8_t COUNTER = 0;

uint32_t Fifo1_Put(char data) {
    if (FULL) return 0;
    if (EMPTY) EMPTY = 0;
    FIFO[Put] = data;
    Put = (Put + 1) % SIZE; //Increments Put circularly
    if (Put == Get) FULL = 1;
    return 1;
}

// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is EMPTY return 0
//         If the FIFO1 has data, remove it, and return it

char Fifo1_Get(void){
    if (EMPTY) return 0;
    if (FULL) FULL = 0;
    char data = FIFO[Get];
    Get = (Get + 1) % SIZE; //Increments Get circularly
    if (Get == Put) EMPTY = 1;
    return data;
}

/* Put using counter
    if(Put == SIZE) Put = 0; // Wrap
    if (COUNTER == SIZE) return 0; // If already at size, fail
    FIFO[Put] = data; // All good, index of put to data, increment put.
    Put++;
    COUNTER++;
    return 1;
*/

/* Get using counter
    if (COUNTER == 0) return 0;
    if (Get == SIZE) Get = 0;
    int ret = FIFO[Get];
    Get++;
    COUNTER--;
    return ret;
*/
