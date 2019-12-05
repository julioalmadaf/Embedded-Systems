/* Program that counts consecutive 1’s */ 
			.text 
			.global 	_start 
_start: 
			LDR 		R1, TEST_NUM 	// load the data word into R1
			
			MOV 		R0, #0 			// R0 will hold the result 
LOOP: 		CMP 		R1, #0 			// loop until the data contains no more 1’s 
			BEQ 		END 
			LSR 		R2, R1, #1 		// perform SHIFT, followed by AND 
			AND 		R1, R1, R2 
			ADD		 	R0, #1 			// count the string lengths so far 
			B 			LOOP

END: 		B 			END

TEST_NUM: 	.word 		0x108FFF61
			.end