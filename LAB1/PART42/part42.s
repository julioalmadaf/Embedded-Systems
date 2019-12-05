/* Program that converts a binary number to decimal */
			.text 
			.global 	_start 
_start: 
			LDR 		R4, =N 
			ADD 		R5, R4, #4 		// R5 points to the decimal digits storage location 
			LDR 		R4, [R4] 		// R4 holds N
			MOV 		R0, R4 			// parameter for DIVIDE goes in R0 
			BL 			DIVIDE 			
			STRB		R3,	[R5, #3]	//1000S IN R3
			STRB		R2,	[R5, #2]	//100S	IN R4
			STRB 		R1, [R5, #1] 	// Tens digit is in R1 	
			STRB 		R0, [R5] 		// Ones digit is in R0
END: 		B 			END

/* Subroutine to perform the integer division R0 / 10. 
* Returns: quotient in R1, and remainder in R0 */ 

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
			
N: 			.word 		7654 				// the decimal number to be converted 
Digits: 	.space 		8 				// storage space for the decimal digits

			.end