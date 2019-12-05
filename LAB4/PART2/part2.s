/* Program that counts consecutive 1’s */ 
.text 
.global 	_start 
_start: 
			
			LDR			R3, =0xFF200050	//base address Pushbuttons
			LDR 		R8, =0xFF200020 // base address of HEX3-HEX0
			LDR 		R9, =0xFF20005C
			MOV			R5,#0
LOOP:		MOV 		R0, R5 			// display R5 on HEX1-0 
			BL 			DIVIDE 			// ones digit will be in R0; tens digit in R1 
			MOV 		R10, R1 		// save the tens digit 
			BL 			SEG7_CODE 
			MOV 		R4, R0 			// save bit code 
			MOV 		R0, R10 		// retrieve the tens digit, get bit code 
			BL 			SEG7_CODE 
			ORR 		R4, R4, R0, LSL #8	//PRINT ON 2 7 SEG
			STR			R4,[R8]				
			LDR			R6,[R9]				//CHECK VALUES OF EDGEREGISTER
			CMP			R6, #0
			BNE			WAIT			//IF BUTTON PRESS PAUSE COUNTER
DO_DELAY: 	LDR 		R7, =200000000 	// delay counter 
SUB_LOOP: 	SUBS 		R7, R7, #1 
			BNE 		SUB_LOOP
			BL			PLUS			//ADDS 1 EVERY .25 SECONDS
			B			LOOP
			
WAIT:		STR			R6, [R9]		//RELEASE INTERRUPT, RESETTING VALUES OF R9
WOW:		LDR			R6,[R9]			//CHECK VALUE OF BUTTONS
			CMP			R6,#0
			BEQ			WOW				//WAIT FOR BUTTON PRESS
			STR			R6, [R9]		//RELEASE INTERRUPT, RESETTING VALUES OF R9
			B			LOOP			

ZERO:		MOV			R5,#0
			BX			LR

PLUS:		ADD			R5,R5,#1		//COUNTER UNTIL 99
			CMP			R5,#100
			BEQ			ZERO
			BX			LR
			
/* Subroutine to convert the digits from 0 to 9 to be shown on a HEX display. 
* Parameters: R0 = the decimal value of the digit to be displayed 
* Returns: R0 = bit patterm to be written to the HEX display */ 
SEG7_CODE: 	LDR 		R1, =BIT_CODES 
			LDRB 		R0, [R1, R0] 
			BX 			LR
			
DIVIDE: 	MOV 		R2, #0 
CONT: 		CMP 		R0, #10 
			BLT 		DIV_END 
			SUB 		R0, #10 
			ADD 		R2, #1 
			B 			CONT 
DIV_END: 	MOV 		R1, R2 			// return quotient in R1 (remainder is in R0) 
			BX 			LR
						
BIT_CODES: 	.byte 		0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110 
			.byte 		0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 
			.skip 		2 				// pad with 2 bytes to maintain word alignment
			.end