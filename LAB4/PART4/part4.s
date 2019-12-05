/* Program that counts consecutive 1’s */ 
.text 
.global 	_start 
_start: 
			
			LDR			R10, =0xFFFEC600	//base address A9 PRIVATE TIMER
			LDR 		R8, =0xFF200020 	// base address of HEX3-HEX0
			LDR 		R9, =0xFF200050		//base address buttons
			
			LDR 		R7, =2000000 		// Load Value
			STR			R7, [R10]
			LDR			R10, =0xFFFEC608	//CONTROL REGISTER
			MOV			R7, #3				//AE bits set to 1, Enabled and A to reload value of load Value
			STR			R7,[R10]
			LDR			R10, =0xFFFEC60C	//Interrupt status
			
			MOV			R5,#0
			
LOOP:		MOV 		R0, R5 				// display R5 on HEX1-0 
			BL 			DIVIDE 				// ones digit will be in R0; tens digit in R1 		
			MOV 		R12, R1 			// save the tens digit 
			BL 			SEG7_CODE 
			MOV 		R4, R0 				// save bit code 
			MOV 		R0, R12 			// retrieve the tens digit, get bit code 
			BL 			SEG7_CODE 
			ORR 		R4, R4, R0, LSL #8	//PRINT ON 2 7 SEG
			MOV			R0,R2
			BL			SEG7_CODE
			ORR 		R4, R4, R0, LSL #16	//PRINT ON 2 7 SEG
			MOV			R0,R3
			BL			SEG7_CODE			
			ORR 		R4, R4, R0, LSL #24	//PRINT ON 2 7 SEG
			STR			R4,[R8]	
			LDR			R6,[R9]
			CMP			R6,#0
			BNE			WAIT
			LDR			R7, [R10]
			CMP			R7, #0
			BEQ			LOOP
			MOV			R7, #1
			STR			R7,[R10]
			BL			PLUS				//ADDS 1 EVERY .01 SECONDS
			B			LOOP	

WAIT:		LDR			R6,[R9]				//SUBROUTINE FOR BUTTON RELEASE
			CMP			R6,#0
			BNE			WAIT
WAIT2:		LDR			R6,[R9]			//SUBROUTINE FOR BUTTON RELEASE
			CMP			R6,#0
			BEQ			WAIT2
DO_DELAY: 	LDR 		R7, =200000000 	// delay counter 
SUB_LOOP: 	SUBS 		R7, R7, #1 
			BNE 		SUB_LOOP
			B			LOOP	

ZERO:		MOV			R5,#0			//resets value to 0
			BX			LR

PLUS:		ADD			R5,R5,#1		//COUNTER UNTIL 99
			LDR			R7, =6000
			CMP			R5, R7
			BEQ			ZERO
			BX			LR
			
/* Subroutine to convert the digits from 0 to 9 to be shown on a HEX display. 
* Parameters: R0 = the decimal value of the digit to be displayed 
* Returns: R0 = bit patterm to be written to the HEX display */ 
SEG7_CODE: 	LDR 		R1, =BIT_CODES 
			LDRB 		R0, [R1, R0] 
			BX 			LR
			
DIVIDE: 	MOV 		R4, #0 
CONT2: 		CMP 		R0, #1000 		//GETS YOU 1000S
			BLT 		DIV_END2 
			SUB 		R0, #1000 
			ADD 		R4, #1 
			B 			CONT2 
DIV_END2: 	MOV 		R3, R4 			 
			MOV			R4, #0
CONT1:		CMP			R0, #100		//GETS YOU 100S
			BLT			DIV_END1
			SUB			R0, #100
			ADD			R4, #1
			B			CONT1
DIV_END1: 	MOV 		R2, R4 			
			MOV			R4, #0
CONT:		CMP			R0, #10			//GETS YOU 10S
			BLT			DIV_END
			SUB			R0, #10
			ADD			R4, #1
			B			CONT	
DIV_END:	MOV			R1, R4			
			BX 			LR				//EXITS SUBROUTINE
						
BIT_CODES: 	.byte 		0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110 
			.byte 		0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 
			.skip 		2 				// pad with 2 bytes to maintain word alignment
			.end