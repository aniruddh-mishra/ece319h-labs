//****************** ECE319K_Lab1.s ***************
// Your solution to Lab 1 in assembly code
// Author: Aniruddh Mishra
// Last Modified: 01/31/2024
// Spring 2024
        .data
        .align 2
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "AM97437" // replace ZZZ123 with your EID here
        .align 2
        .global Phase
Phase:  .long 10 // 0 for info, 1-6 for debug, 10 for grade

        .global Lab1
// Input: R0 points to the list
// Return: R0 as specified in Lab 1 assignment and terminal window
// According to AAPCS, you must save/restore R4-R7
// If your function calls another function, you must save/restore LR
Lab1: PUSH {R4-R7,LR}
       // your solution goes here

	MOVS R3, #0

	ITERATE:
		LDR R4, =EID
		LDR R5, [R0]
		ANDS R5, R5
		BEQ FAIL
		BL CHECK
		BNE SUCCESS
		ADDS R3, #1
		ADDS R0, R0, #8
		B ITERATE

	FAIL:
		MOVS R0, #0
		SUBS R0, #1
		B END

	SUCCESS:
		MOVS R0, #0
		ADDS R0, R0, R3
		B END

	CHECK:
		// Input: R5 is the pointer to EID in list and R4 is pointer to student's EID
		// Output: R7 will store a 0 if these strings are not equal and a 1 if they are
		NEXTCHAR:
			LDRB R6, [R5]
			LDRB R7, [R4]
			SUBS R7, R7, R6
			BNE NOTEQUAL
			ANDS R6, R6
			BEQ EQUAL
			ADDS R5, R5, #1
			ADDS R4, R4, #1
			B NEXTCHAR

		EQUAL:
			MOVS R7, #1
			BX LR

		NOTEQUAL:
			MOVS R7, #0
			BX LR

	END:
    	POP  {R4-R7,PC} // return


        .end
