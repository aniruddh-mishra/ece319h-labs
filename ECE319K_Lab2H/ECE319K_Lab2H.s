//****************** ECE319K_Lab2H.s ***************
// Your solution to Lab 2 in assembly code
// Author: Your name
// Last Modified: Your date
// ECE319H Spring 2024 (ECE319K students do Lab2)


   .equ GPIOB_DOE31_0,  0x400A32C0
   .equ GPIOB_DOUT31_0, 0x400A3280
   .equ GPIOB_DIN31_0,  0x400A3380
   .equ GPIOB_DOUTSET31_0, 0x400A3290
   .equ GPIOB_DOUTCLR31_0, 0x400A32A0
   .equ GPIOB_DOUTTGL31_0, 0x400A32B0
   .equ IOMUXPB0,       0x40428004+4*11
   .equ IOMUXPB1,       0x40428004+4*12
   .equ IOMUXPB2,       0x40428004+4*14
   .equ IOMUXPB3,       0x40428004+4*15
   .equ IOMUXPB16,      0x40428004+4*32
   .equ IOMUXPB17,      0x40428004+4*42
   .equ IOMUXPB18,      0x40428004+4*43

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
  .equ dot,100
  .equ dash,(3*dot)
  .equ shortgap,(2*dot)  // because it will send an interelement too
  .equ interelement,dot
  Morse:
  .long  dot,  dash,    0,    0, 0 // A
  .long  dash,  dot,  dot,  dot, 0 // B
  .long  dash,  dot, dash,  dot, 0 // C
  .long  dash,  dot,  dot,    0, 0 // D
  .long  dot,     0,    0,    0, 0 // E
  .long  dot,   dot, dash,  dot, 0 // F
  .long  dash, dash,  dot,    0, 0 // G
  .long  dot,   dot,  dot,  dot, 0 // H
  .long  dot,   dot,    0,    0, 0 // I
  .long  dot,  dash, dash, dash, 0 // J
  .long  dash,  dot, dash,    0, 0 // K
  .long  dot,  dash,  dot,  dot, 0 // L
  .long  dash, dash,    0,    0, 0 // M
  .long  dash,  dot,    0,    0, 0 // N
  .long  dash, dash, dash,    0, 0 // O
  .long  dot,  dash, dash,  dot, 0 // P
  .long  dash, dash,  dot, dash, 0 // Q
  .long  dot,  dash,  dot,    0, 0 // R
  .long  dot,   dot,  dot,    0, 0 // S
  .long  dash,    0,    0,    0, 0 // T
  .long  dot,   dot, dash,    0, 0 // U
  .long  dot,   dot,  dot, dash, 0 // V
  .long  dot,  dash, dash,    0, 0 // W
  .long  dash,  dot,  dot, dash, 0 // X
  .long  dash,  dot, dash, dash, 0 // Y
  .long  dash, dash,  dot,  dot, 0 // Z

  .align 2
  .global Lab2Grader
  .global Lab2
  .global Debug_Init // Lab3 programs
  .global Dump       // Lab3 programs

// Switch input: PB2 PB1 or PB0, depending on EID
// LED output:   PB18 PB17 or PB16, depending on EID
// logic analyzer pins PB18 PB17 PB16 PB2 PB1 PB0
// analog scope pin PB20
Lab2:
// Initially the main program will
//   set bus clock at 80 MHz,
//   reset and power enable both Port A and Port B
// Lab2Grader will
//   configure interrupts  on TIMERG0 for grader or TIMERG7 for TExaS
//   initialize ADC0 PB20 for scope,
//   initialize UART0 for grader or TExaS
	BL Lab2Init	// you initialize input pin and output pin
	MOVS R0,#3
	BL Lab2Grader
	BL Debug_Init

MAIN:
	//LDR R3, =GPIOB_DIN31_0
	//LDR R1, [R3]
	//MOVS R2, #1
	//LSLS R2, #2
	//ANDS R1, R2
	//BNE PRINT
	LDR R0, =80
	B PRINT
	B MAIN

PRINT:
	//MOVS R0,#10
	//BL Lab2Grader
	SUBS R0, 0x41
	MOVS R1, #20
	MULS R0, R0, R1
	LDR R4, =Morse
	ADDS R4, R0
	LETTER:
		LDR R0, [R4]
		ANDS R0, R0
		BEQ RETURN
		PUSH {R0}
		BL LEDON
		POP {R0}
		BL DELAY
		BL LEDOFF
		LDR R0, =interelement
		BL DELAY
		ADDS R4, #4
		B LETTER

	RETURN:
		//LDR R0, =shortgap
		//BL DELAY
		B MAIN

LEDOFF:
	LDR R0, =GPIOB_DOUT31_0
	LDR R1, [R0]
	MOVS R2, #1
	LSLS R2, R2, #17
	BICS R1, R2
	STR R1, [R0]
	PUSH {LR}
	BL  Dump
	POP {PC}

LEDON:
	LDR R0, =GPIOB_DOUT31_0
	LDR R1, [R0]
	MOVS R2, #1
	LSLS R2, R2, #17
	ORRS R1, R2
	STR R1, [R0]
	PUSH {LR}
	BL  Dump
	POP {PC}

// make switch input, LED output
// PortB is already reset and powered
// Set IOMUX for your input and output
// Set GPIOB_DOE31_0 for your output (be friendly)
Lab2Init:
   LDR R1, =0x00040081
   LDR R0, =IOMUXPB2
   STR R1, [R0]
   LDR R1, =0x00000081
   LDR R0, =IOMUXPB17
   STR R1, [R0]
   LDR R0, =GPIOB_DOE31_0
   MOVS R1, #1
   LSLS R1, R1, #17
   LDR R2, [R0]
   ORRS R1, R1, R2
   STR R1, [R0]
   BX   LR

DELAY:
	Another:
		LDR R1, =80000
		SUBS R1, R1, #2
		dloop:
			SUBS R1, R1, #4
			NOP
			BHS dloop
		SUBS R0, R0, #1
	CMP R0, #0
	BNE Another
	BX LR

   .end
