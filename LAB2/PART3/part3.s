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

TEST_NUM: 	.word 		0x103fe00f, 0xAAAAAAA0, 0x00FFFF00, 0xABCDEFFE
			.word		0X10203040, 0X12345678, 0X9ABCDEF0,	0X00000001
			.word		0XFEDCBA12, 0X0000000A
			
N:			.word		10
			.end