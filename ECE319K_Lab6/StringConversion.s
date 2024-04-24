// StringConversion.s
// Student names: Aniruddh Mishra & Evan Lai
// Last modification date: 03/20/2024
// Professor: Dr. Tiwari

// Runs on any Cortex M0
// ECE319K lab 6 number to string conversion
//
// You write udivby10 and Dec2String
     .data
     .align 2
// no globals allowed for Lab 6
    .global OutChar    // virtual output device
    .global OutDec     // your Lab 6 function
    .global Test_udivby10

    .text
    .align 2
// **test of udivby10**
// since udivby10 is not AAPCS compliant, we must test it in assembly
Test_udivby10:
    PUSH {LR}

    MOVS R0,#123
    BL   udivby10
// put a breakpoint here
// R0 should equal 12 (0x0C)
// R1 should equal 3

    LDR R0,=12345
    BL   udivby10
// put a breakpoint here
// R0 should equal 1234 (0x4D2)
// R1 should equal 5

    MOVS R0,#0
    BL   udivby10
// put a breakpoint here
// R0 should equal 0
// R1 should equal 0
    POP {PC}

//****************************************************
// divisor=10
// Inputs: R0 is 16-bit dividend
// quotient*10 + remainder = dividend
// Output: R0 is 16-bit quotient=dividend/10
//         R1 is 16-bit remainder=dividend%10 (modulus)
// not AAPCS compliant because it returns two values
udivby10:
   PUSH {LR}
   
   LDR R1, =0xA000 // 10 shifted to the left by 12
   LDR R2, =0x2000 // Bit Mask; shifted to left by 1
   MOVS R3, #0 // Quotient

   LOOP:
   	LSRS R2, R2, #1
   	CMP R0, R1
   	BLO SKIP // Divisor is smaller than dividend at this bit
   	SUBS R0, R1
   	ORRS R3, R2
   	SKIP:
   		LSRS R1, #1
		CMP R2, #1
   		BNE LOOP

   MOVS R1, R0
   MOVS R0, R3

   POP  {PC}

  
//-----------------------OutDec-----------------------
// Convert a 16-bit number into unsigned decimal format
// ECE319H students must use recursion
// Call the function OutChar to output each character
// OutChar does not do actual output, OutChar does virtual output used by the grader
// You will call OutChar 1 to 5 times
// Input: R0 (call by value) 16-bit unsigned number
// Output: none
// Invariables: This function must not permanently modify registers R4 to R11

// 1. Binding phase
.equ inputNum, 0
.equ printNum, 2
.equ printChar, 4

OutDec:
    PUSH {R7, LR}

    SUB SP, #8 // 2. Allocation phase
	MOV R7, SP // Creating frame pointer

	// 3. Access Phase
	STRH R0, [R7, #inputNum]

	CMP R0, #10
	BHS RECURSIVECASE
	ADDS R0, #48
	STRH R0, [R7, #inputNum]
	BL OutChar
	B RETURN

	RECURSIVECASE:
		BL udivby10
		STRH R0, [R7, #inputNum]
		STRH R1, [R7, #printNum]
		BL OutDec
		LDRH R1, [R7, #printNum]

		ADDS R1, #48
		STRH R1, [R7, #printNum]

		MOVS R0, R1
		STRB R0, [R7, #printChar]
		BL OutChar
		B RETURN

	RETURN:
		// 4. Deallocation Phase
		ADD SP, #8
    	POP  {R7, PC}
//* * * * * * * * End of OutDec * * * * * * * *



     .end
