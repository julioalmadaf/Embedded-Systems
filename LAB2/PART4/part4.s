/* Program that counts consecutive 1’s */ 
			.text 
			.global 	_start 
_start: 
			

			MOV			R5, #0			//initialize registers
			MOV			R6, #0			
			MOV			R7, #0
			BL			ONES
			BL			ZEROES
			BL			ALTER
			/* Display R5 on HEX1-0, R6 on HEX3-2 and R7 on HEX5-4 */
DISPLAY: 	LDR 		R8, =0xFF200020 // base address of HEX3-HEX0 
			MOV 		R0, R5 			// display R5 on HEX1-0 
			BL 			DIVIDE 			// ones digit will be in R0; tens digit in R1 
			MOV 		R10, R1 		// save the tens digit 
			BL 			SEG7_CODE 
			MOV 		R4, R0 			// save bit code 
			MOV 		R0, R10 		// retrieve the tens digit, get bit code 
			BL 			SEG7_CODE 
			ORR 		R4, R4, R0, LSL #8	//COMBINATION NECESSARY FOR HEX1-0
			MOV 		R0, R6 			// display R6 on HEX3-2 
			BL 			DIVIDE 			// ones digit will be in R0; tens digit in R1 
			MOV 		R10, R1 		// save the tens digit 
			BL 			SEG7_CODE 
			MOV 		R3, R0 			// save bit code 
			MOV 		R0, R10 		// retrieve the tens digit, get bit code 
			BL 			SEG7_CODE 
			ORR 		R3, R3, R0, LSL #8	//COMBINATION NECESSARY FOR HEX3-2
			ORR			R4, R4, R3, LSL #16	//PUT TOGETHER EVERYTHING SO YOU CAN PRINT ON HEX3-HEX0
			STR 		R4, [R8]		// display the numbers from R6 and R5
			LDR 		R8, =0xFF200030 // base address of HEX5-HEX4 
			MOV 		R0, R7 			// display R5 on HEX1-0 
			BL 			DIVIDE 			// ones digit will be in R0; tens digit in R1 
			MOV 		R10, R1 		// save the tens digit 
			BL 			SEG7_CODE 
			MOV 		R4, R0 			// save bit code 
			MOV 		R0, R10 		// retrieve the tens digit, get bit code 
			BL 			SEG7_CODE 
			ORR 		R4, R4, R0, LSL #8
			STR 		R4, [R8]		// display the numbers from R6 and R5
			
			
END: 		B 			END		
			

ONES:		LDR 		R1, TEST_NUM 	// load the data word into R1
			LDR			R3, =TEST_NUM	
			LDR			R0, =N
			MOV			R8, #0
			LDR			R8,[R0]
LOOP:		MOV 		R0, #0 			// R0 will hold the NEW result 
			CMP			R8, #0			//If ALL WORDS CHECKED DONE11
			BEQ			DONE11	
SCH: 		CMP 		R1, #0 			// loop until the data contains no more 1’s 
			BEQ 		DONE1 
			LSR 		R2, R1, #1 		// perform SHIFT, followed by AND 
			AND 		R1, R1, R2 
			ADD		 	R0, #1 			// count the string lengths so far 
			B 			SCH
DONE1:		ADD			R3, R3, #4		//NEXT NUM
			LDR			R4, [R3]
			MOV			R1, R4
			SUBS		R8, R8, #1		//DECREMENT COUNTER
			CMP			R0, R5			//COMPARE LARGEST NUMS
			BLE			LOOP
			MOV			R5, R0			//UPDATE LARGEST STRING OF 1S
			B			LOOP
DONE11:		BX			LR

ZEROES:		LDR 		R1, TEST_NUM 	// load the data word into R1
			NEG			R1,	R1				//Turn 0s to 1s
			SUB			R1,	#1				//ADJUST CONVERSION
			LDR			R3, =TEST_NUM
			LDR			R0, =N				
			MOV			R8, #0
			LDR			R8,[R0]
LOOP2:		MOV 		R0, #0 			// R0 will hold the result 
			CMP			R8, #0			
			BEQ			DONE22	
SCH2: 		CMP 		R1, #0 			// loop until the data contains no more 1’s 
			BEQ 		DONE2 
			LSR 		R2, R1, #1 		// perform SHIFT, followed by AND 
			AND 		R1, R1, R2 
			ADD		 	R0, #1 			// count the string lengths so far 
			B 			SCH2
DONE2:		ADD			R3, R3, #4
			LDR			R4, [R3]
			NEG			R1, R4
			SUB			R1,	#1
			SUBS		R8, R8, #1
			CMP			R0, R6
			BLE			LOOP2
			MOV			R6, R0			//UPDATE LARGEST STRING OF 0S
			B			LOOP2
DONE22:		BX			LR

ALTER:		LDR 		R1, TEST_NUM 	// load the data word into R1
			LDR			R3, =TEST_NUM	
			LDR			R0, =N
			MOV			R8, #0
			LDR			R8,[R0]
LOOP3:		MOV 		R0, #0 			// R0 will hold the result 
			CMP			R8, #0			
			BEQ			DONE33	
SCH3: 		LSR 		R2, R1, #1 		// perform SHIFT, followed by XOR 
			EOR 		R1, R1, R2		//XOR CREATES 1S, SO THEN YOU JUST HAVE TO COUNT CONSECUTIVE 1S
LS3:		CMP 		R1, #0 			// loop until the data contains no more 1’s 
			BEQ 		DONE3 
			LSR 		R2, R1, #1 		// perform SHIFT, followed by AND 
			AND 		R1, R1, R2 		
			ADD		 	R0, #1 			// count the string lengths so far 
			B 			LS3
DONE3:		ADD			R3, R3, #4
			LDR			R4, [R3]
			MOV			R1, R4
			SUBS		R8, R8, #1
			CMP			R0, R7
			BLE			LOOP3
			MOV			R7, R0			//UPDATE LARGEST STRING OF ALTERNATES
			B			LOOP3
DONE33:		BX			LR

/* Subroutine to convert the digits from 0 to 9 to be shown on a HEX display. 
* Parameters: R0 = the decimal value of the digit to be displayed 
* Returns: R0 = bit patterm to be written to the HEX display */ 
SEG7_CODE: 	LDR 		R1, =BIT_CODES 
			LDRB 		R0, [R1, R0] 
			BX 			LR

DIVIDE: 	MOV 		R2, #0 			//TO GET 10S AND 1S
CONT: 		CMP 		R0, #10 
			BLT 		DIV_END 
			SUB 		R0, #10 
			ADD 		R2, #1 
			B 			CONT 
DIV_END: 	MOV 		R1, R2 			// return quotient in R1 (remainder is in R0) 
			BX 			LR

TEST_NUM: 	.word 		0x103fe00f, 0xAAAAAAA0, 0x00FFFF00, 0xABCDEFFE
			.word		0X10203040, 0X12345678, 0X9ABCDEF0,	0X00000001
			.word		0XFEDCBA12, 0X0000000A
			
N:			.word		10
			
BIT_CODES: 	.byte 		0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110 
			.byte 		0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 
			.skip 		2 				// pad with 2 bytes to maintain word alignment
			.end