/* Program that counts consecutive 1’s */ 
.text 
.global 	_start 
_start: 
			

			LDR			R3, =0xFF200050	//base address Pushbuttons
			LDR 		R8, =0xFF200020 // base address of HEX3-HEX0 
			MOV			R5,#0
LOOP:		MOV			R0, R5
			BL			SEG7_CODE
			MOV			R4,R0
			STR			R4,[R8]			//PRINT IN 7 SEG
			LDR			R6,[R3]
			CMP			R6, #1			//CHECK WISH BUTTON WAS PRESSED
			BEQ			ZERO
			CMP			R6, #2
			BEQ			PLUS
			CMP			R6, #4
			BEQ			LESS
			CMP			R6, #8
			BEQ			NADA
			B			LOOP			//ALWAYS CHECK BUTTONS

ZERO:		MOV			R5,#0			//SET 7 SEG TO 0
			BL			WAIT
			B 			LOOP

PLUS:		ADD			R5,R5,#1		//ADD 1 TO COUNTER
			CMP			R5,#10
			BEQ			ZERO	
			BL			WAIT
			B			LOOP
			
LESS:		SUBS 		R5,R5,#1		//SUBS 1 TO COUNTER
			CMP			R5,#0
			BLE			ZERO			//IF -1 LEAVE AT 0
			BL			WAIT
			B 			LOOP

NADA:		BL			WAIT			//BLANK AND WAIT FOR BUTTON PRESS
			MOV			R0,#0
			MOV			R4, R0
			STR			R4,[R8]			//BLANK 7 SEG
WAITNADA:	LDR			R6,[R3]			
			CMP			R6,#0
			BEQ			WAITNADA		//WAIT FOR BUTTON PRESS
			B			ZERO			//SET TO ZERO WHEN BUTTON PRESS

/* Subroutine to convert the digits from 0 to 9 to be shown on a HEX display. 
* Parameters: R0 = the decimal value of the digit to be displayed 
* Returns: R0 = bit patterm to be written to the HEX display */ 
SEG7_CODE: 	LDR 		R1, =BIT_CODES 
			LDRB 		R0, [R1, R0] 
			BX 			LR

WAIT:		LDR			R6,[R3]			//SUBROUTINE FOR BUTTON RELEASE
			CMP			R6,#0
			BNE			WAIT
			BX			LR
			
						
BIT_CODES: 	.byte 		0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110 
			.byte 		0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 
			.skip 		2 				// pad with 2 bytes to maintain word alignment
			.end