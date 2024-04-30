// FIFO.h
// Runs on any microcontroller
// Student names: Aniruddh Mishra
// Last Modified: 1/1/2024


#ifndef __FIFO_H__
#define __FIFO_H__
#include <stdint.h>

#define FIFOSIZE 32  // maximum storage is FIFO_SIZE-1 elements
class Queue{
private:
  uint8_t Buf[FIFOSIZE];
  int PutI; // index to an empty place, next place to put
  int GetI; // index to oldest data, next to get

public:
  Queue();            // initialize queue
  bool IsEmpty(void); // true if empty
  bool IsFull(void);  // true if full
  bool Put(uint8_t x);   // enter data into queue
  bool Get(uint8_t *pt); // remove data from queue
};


#endif //  __FIFO_H__
