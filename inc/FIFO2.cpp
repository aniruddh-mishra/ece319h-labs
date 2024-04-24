// FIFO2.cpp
// Runs on any microcontroller
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The file includes a transmit FIFO
// using index implementation and a receive FIFO using pointer
// implementation.  Other index or pointer implementation FIFOs can be
// created using the macros supplied at the end of the file.
// Created: 1/16/2020 
// Student names: Aniruddh Mishra
// Last modification date: 04/10/2024

#include <stdio.h>
#include <stdint.h>

#include "../inc/FIFO2.h"
#include "../inc/ST7735.h"


// A class named Queue that defines a FIFO
Queue::Queue(){
  PutI = 0;
  GetI = 0;
}

// To check whether Queue is empty or not
bool Queue::IsEmpty(void){
    if (GetI == PutI) return true;
    return false;
}

  // To check whether Queue is full or not
bool Queue::IsFull(void){
    if ((PutI + 1) % FIFOSIZE == GetI) return true;
    return false;
}

  // Inserts an element in queue at rear end
bool Queue::Put(char x){
    if (IsFull()) return false;
    Buf[PutI] = x;
    PutI = (PutI + 1) % FIFOSIZE;
    return true;
}

  // Removes an element in Queue from front end. 
bool Queue::Get(char *pt){
    if (IsEmpty()) return false;
    *pt = Buf[GetI];
    GetI = (GetI + 1) % FIFOSIZE;
    return true;
}

  /* 
     Printing the elements in queue from front to rear. 
     This function is only to test the code. 
     This is not a standard function for Queue implementation. 
  */
void Queue::Print(void){
    for (int i = GetI; i != PutI; i = (i + 1) % FIFOSIZE) {
        printf("%c\n", Buf[i]);
    }
}


