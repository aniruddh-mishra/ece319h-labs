// Lab6Grader.h
// Runs on MSPM0G3507
// Periodic timer Timer3A and Timer5A which will interact with debugger and implement logic analyzer 
// It initializes on reset and runs whenever interrupts are enabled
// Jonathan Valvano
// 10/9/2023

/* 
 Copyright 2023 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>

// mode = 1 to test SPIOutCommand, no grading
// mode = 2 to test SPIOutData, no grading
// mode = 3 to test GradeDec2String, no grading
// mode = 4 to test all three with grading
void Lab6Grader(int mode);

