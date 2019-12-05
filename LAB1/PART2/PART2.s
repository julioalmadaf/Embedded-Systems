/* Program that ﬁnds the largest number in a list of integers */ 
				
				.text 
				
				.global 	_start 

_start: 
				LDR 		R4, =RESULT 	// R4 points to result location 
				LDR 		R2, [R4, #4] 	// R2 holds the number of elements in the list 
				ADD 		R3, R4, #8 		// R3 points to the ﬁrst number 
				LDR 		R0, [R3] 		// R0 holds the largest number so far, (THE VALUE R3 IS HOLDING RIGHT NOW)

LOOP: 			SUBS 		R2, R2, #1 		// decrement the loop counter, S for condition flags
				BEQ 		DONE 			// checks if zero flag is set
				ADD 		R3, R3, #4		
				LDR 		R1, [R3] 		// get the next number 
				CMP 		R0, R1 			// check if larger number found 
				BGE 		LOOP 
				MOV 		R0, R1 			// update the largerst number 
				B 			LOOP

DONE: 			STR 		R0, [R4] 		// store largest number into result location

END: 			B 			END

RESULT: 		.word 		0 

N: 				.word 		7 				// number of entries in the list 

NUMBERS: 		.word 		4, 5, 3, 6 		// the data 
				.word 		9, 8, 2
				
				.end
