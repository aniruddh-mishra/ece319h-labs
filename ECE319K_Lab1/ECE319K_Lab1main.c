/* ECE319K_Lab1main.c
 * Jonathan Valvano
 * January 3, 2024
 * Derived from uart_rw_multibyte_fifo_poll_LP_MSPM0G3507_nortos_ticlang
 */

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
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include <stdio.h>
#include <string.h>

// PA10 is UART0 Tx    index 20 in IOMUX PINCM table
// PA11 is UART0 Rx    index 21 in IOMUX PINCM table
// Insert jumper J25: Connects PA10 to XDS_UART
// Insert jumper J26: Connects PA11 to XDS_UART
uint32_t Score,Option;
int32_t Correct[8],YourAns;
#define DEBUG 0

uint32_t M;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}
char setUpper(char in){
  if((in >= 'a')&&(in <= 'z')){
    return in-0x20;
  }
  return in;
}
extern const char EID[16]; // student's EID
extern const uint32_t Phase;
uint32_t IndexAtNumber;
uint32_t SizeofEID;
char SecondLetter;

const char Opt1[]="Option 1: Return R0 equal to the Score based on your EID, return R0 equal to -1 if your EID is not in the list.";
const char Opt2[]="Option 2: Return R0 equal to the index value of your EID, return R0 equal to -1 if your EID is not in the list.";
const char Opt3[]="Option 3: Return R0 equal to the number of characters in the shortest EID in the list.";
const char Opt4[]="Option 4: Return R0 equal to the number of characters in the longest EID in the list.";
const char Opt5[]="Option 5: Return R0 equal to the number of students with the same letters as your EID.";
const char Opt6[]="Option 6: Return R0 equal to the number of students with EIDs that are same number of characters as your EID.";
//const char Opt7[]="Option 7: Look for a record with a score of 95. Return R0 equal to the number of characters in the EID in the first entry found with score 95.";
//const char Opt8[]="Option 8: Return R0 equal to the number of students with the same numbers as your EID.";
struct student{
  char *eid; // pointer EID string
  int32_t grade;
};
typedef struct student student_t;
char EIDs[10][20];
char student[20];
char student1[20];
char student2[20];
student_t List[10]; // up to 9 students
/*void UART_OutSDec(int32_t n){
  if(n<0){
    UART_OutChar('-'); n = -n;
  }
  UART_OutUDec((uint32_t)n);
}*/
void BuildEID(int i){int j=2; int n;
  EIDs[i][0] = 'A'+Random(26);
  EIDs[i][1] = SecondLetter; SecondLetter += 2;
  if(Random(100) > 49){
    EIDs[i][2] = 'A'+Random(26); j++;
  }
  n = Random(6); // one plus 0 to 5 numbers more
  EIDs[i][j] = '1'+Random(9); j++;
  for(;n;n--){
    EIDs[i][j] = '0'+Random(10); j++;
  }
  EIDs[i][j] = 0;
  if(strcmp(student, EIDs[i])==0){ // make sure student didn't choose student by accident
    EIDs[i][1]++;
  }
}
void OptFill1(void){int i,j;
  SecondLetter = 'B'; // makes sure all EIDs are unique
  for(i=0;i<5;i++){ // number of students
    BuildEID(i);
    List[i].eid = EIDs[i];
    List[i].grade = 80+Random(21); // 80-100
  }
  j = Random(4)+1;
  List[j].eid = student;
  Correct[0] = List[j].grade; // your grade
  Correct[1] = j;  // index of your EID
  if(j < 2){
    List[j+1].eid = student2;
  }else{
    List[j-1].eid = student2;
  }
  List[i].eid = 0;
  List[i].grade = Random(80)+1;
}
void OptFill2(void){int i,j;
  SecondLetter = 'C'; // makes sure all EIDs are unique
  for(i=0;i<7;i++){ // number of students
    BuildEID(i);
    List[i].eid = EIDs[i];
    List[i].grade = 80+Random(21); // 80-100
  }
  j = Random(6)+1;
  List[j].eid = student;
  Correct[0] = List[j].grade; // your grade
  Correct[1] = j;  // index of your EID
  if(j < 4){
    List[j+2].eid = student1;
    List[j+1].eid = student2;
  }else{
    List[j-3].eid = student1;
    List[j-1].eid = student2;
  }
  List[i].eid = 0;
  List[i].grade = Random(80)+1;
}
void OptFill3(void){int i;
  SecondLetter = 'A'; // makes sure all EIDs are unique
  for(i=0;i<9;i++){ // number of students
    BuildEID(i);
    List[i].eid = EIDs[i];
    List[i].grade = 80+Random(21); // 80-100
  }
// first

  List[3].eid = student2;
  List[0].eid = student;
  Correct[0] = List[0].grade;// your grade
  Correct[1] = 0;  // index of your EID
  List[i].eid = 0;
  List[i].grade = Random(80)+1;
}
void OptFill4(void){int i;
  SecondLetter = 'B'; // makes sure all EIDs are unique
  for(i=0;i<9;i++){   // number of students
    BuildEID(i);
    List[i].eid = EIDs[i];
    List[i].grade = 80+Random(21); // 80-100
  }
  List[8].eid = student1;
  List[2].eid = student2;

  Correct[0] = -1; // not there, your grade
  Correct[1] = -1; // index of your EID
  List[i].eid = 0;
  List[i].grade = Random(80)+1;
}
void OptFill5(void){
  Correct[0] = -1; // not there
  Correct[1] = -1; // index of your EID
  List[0].eid = 0;
  List[0].grade = Random(80)+1;
}
void OptFill6(void){int i,j;
  SecondLetter = 'C'; // makes sure all EIDs are unique
  for(i=0;i<9;i++){ // number of students
    BuildEID(i);
    List[i].eid = EIDs[i];
    List[i].grade = 80+Random(21); // 80-100
  }
  j = Random(9);
  List[j].eid = student;
  Correct[0] = List[j].grade; // your grade
  Correct[1] = j;  // index of your EID
  if(j < 4){
    List[j+2].eid = student1;
    List[j+1].eid = student2;
  }else{
    List[j-3].eid = student1;
    List[j-1].eid = student2;
  }
  List[i].eid = 0;
  List[i].grade = Random(80)+1;
}
void Show(uint32_t testcase){
  UART_OutString("\n\rTest case "); UART_OutUDec(testcase); UART_OutString(" : ");
  int j=0;
  while(List[j].eid[0]){
    UART_OutString("{\"");UART_OutString(List[j].eid);UART_OutString("\", ");UART_OutSDec(List[j].grade);UART_OutString("},");
    j++;
  }
  UART_OutString(" Answer="); UART_OutSDec(Correct[Option-1]);
}
int32_t Lab1(student_t List[]);
void Run(void){int mistakes=0;
  if(Option > 6) return;
  switch(Option){
    case 1 : UART_OutString((char*)Opt1); break;
    case 2 : UART_OutString((char*)Opt2); break;
    case 3 : UART_OutString((char*)Opt3); break;
    case 4 : UART_OutString((char*)Opt4); break;
    case 5 : UART_OutString((char*)Opt5); break;
    case 6 : UART_OutString((char*)Opt6); break;
  }
  if(Phase == 0){
    OptFill1(); Show(1);
    OptFill2(); Show(2);
    OptFill3(); Show(3);
    OptFill4(); Show(4);
    UART_OutString("\n\rTest case 5 : Empty, Answer = -1");
  //  OptFill6(); Show(6); // hidden test case
  }else if(Phase == 1){
    UART_OutString("\n\rPhase 1 ");
    OptFill1(); //Show(1);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 2){
    UART_OutString("\n\rPhase 2 ");
    OptFill1(); OptFill2(); //Show(2);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 3){
    UART_OutString("\n\rPhase 3 ");
    OptFill1(); OptFill2(); OptFill3(); //Show(3);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 4){
    UART_OutString("\n\rPhase 4 ");
    OptFill1(); OptFill2(); OptFill3(); OptFill4(); //Show(4);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 5){
    UART_OutString("\n\rPhase 5 ");
    OptFill1(); OptFill2(); OptFill3(); OptFill4(); OptFill5(); //Show(5);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 6){
    UART_OutString("\n\rPhase 6 ");
    OptFill1(); OptFill2(); OptFill3(); OptFill4(); OptFill5(); OptFill6(); //Show(6);
    YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      UART_OutString("Correct");
    }else{
      UART_OutString("Incorrect");
    }
  }else if(Phase == 10){
    OptFill1(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+20;
    }else{
      mistakes++;
      UART_OutString(" Case 1 incorrect.");
    }
    OptFill2(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+20;
    }else{
      mistakes++;
      UART_OutString(" Case 2 incorrect.");
    }
    OptFill3(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+20;
    }else{
      mistakes++;
      if(mistakes < 3){
        UART_OutString(" Case 3 incorrect.");
      }
    }
    OptFill4(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+10;
    }else{
      mistakes++;
      if(mistakes < 3){
        UART_OutString(" Case 4 incorrect.");
      }
    }
    OptFill5(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+10;
    }else{
      mistakes++;
      if(mistakes < 3){
        UART_OutString(" Case 5 incorrect.");
      }
    }
    OptFill6(); YourAns = Lab1(List);
    if(YourAns == Correct[Option-1]){
      Score = Score+20;
    }else{
      mistakes++;
      if(mistakes < 3){
        UART_OutString(" Case 6 incorrect.");
      }
    }
    if(mistakes == 0){
      UART_OutString("\n\rDone!");
    }else{
      UART_OutString("\n\rSome bugs");
    }
    UART_OutString(" Score = "); UART_OutUDec(Score);
  }

}
void EndLab1(void){
  UART_OutString("\n\rEnd of Lab 1, Spring 2024\n\r");
  while(1){
  }
}
uint32_t Dummy;
int main(void){
  Clock_Init40MHz();
  LaunchPad_Init();
  UART_Init();
  if(strcmp(EID,"ZZZ123")==0){
    UART_OutString("Please enter your EID in ECE319K_Lab1.s");
    EndLab1();
  }
  SizeofEID = 0;
  IndexAtNumber = 16;
  M = 5; // seed changes only with EID
  for(int i=0; EID[i]; i++){ char letter;
    letter = EID[i];
    if(IndexAtNumber == 16){ // looking at letters
      if((letter >= '0')&&(letter <= '9')){
        IndexAtNumber = i;
      }else{
        if((letter < 'A')||(letter > 'Z')){
          UART_OutString("Please your EID in all CAPs in ECE319K_Lab1.s");
          EndLab1();
        }
      }
    }else{ // looking at numbers
      if((letter < '0')||(letter > '9')){
        UART_OutString("Please enter your EID as letters followed by numbers in ECE319K_Lab1.s");
        EndLab1();
      }
    }
    SizeofEID++;
    M = M*letter;
  }
  if(SizeofEID>10){
    UART_OutString("EID too long");
    EndLab1();
  }
  UART_OutString("\n\rECE319K Lab 1, Spring 2024\n\rStudent EID= ");
  strcpy(student,EID);
  strcpy(student1,EID);
  if(student1[IndexAtNumber] < '5'){
    student1[IndexAtNumber] +=3;
  }else{
    student1[IndexAtNumber] -=3;
  }
  strcpy(student2,EID);
  if(student2[IndexAtNumber] < '5'){
    student2[IndexAtNumber]++;
  }else{
    student2[IndexAtNumber]--;
  }
  UART_OutString(student);
  if(Phase == 0){
    UART_OutString("\n\rPhase 0: Just showing input lists and expected outputs\n\r");
  }else if(Phase == 10){
    UART_OutString("\n\rRunning the grader\n\r");
  }else if(Phase <= 6){
    UART_OutString("\n\rCalling your Lab 1 with input case=");
    UART_OutUDec(Phase);
    UART_OutString("\n\r");
  }else{
    UART_OutString("\n\rPhase must be 0 to 6 or 10");
    EndLab1();
  }
  Score = 0;
  Dummy = Random(100);
  if(Random(100) & 0x02){ // 0 or 2
    Option = 1;
  }else{
    Option = 2;
  }
  Run();
  EndLab1();
  /*
  UART_OutString((char*)Opt2);
  UART_OutString((char*)Opt3);
  UART_OutString((char*)Opt4);
  UART_OutString((char*)Opt5);
  UART_OutString((char*)Opt6);
  UART_OutString((char*)Opt7);
  UART_OutString((char*)Opt8);
  */
  while(1){
  }
}
