			.text 
			.global _start
_start:
			LDR 	R4, =RESULT 		// R4 points to result location 
			LDR 	R0, [R4, #4] 		// R0 holds the number of elements in the list 
			ADD 	R1, R4, #8 			// R1 points to the ﬁrst number 
			LDR		R2,[R1]
			BL 		LARGE 				//CALL SUBROUTINE
			STR 	R0, [R4]			// R0 holds the subroutine return value

END: 		B 		END

LARGE: 		SUBS 	R0,R0, #1			//decrement loop counter
			BEQ		DONE
			ADD		R1,R1,#4			
			LDR		R3,[R1]				//get the next number
			CMP		R2,R3
			BGE		LARGE
			MOV		R2,R3				//update largest number
			B		LARGE
DONE:		MOV		R0,R2				//Move Largest number to R0
			BX		LR
			
RESULT: 	.word 	0 
N: 			.word 	7 					// number of entries in the list 
NUMBERS: 	.word 	4, 5, 3, 6 			// the data 
			.word 	1, 8, 2
.end
