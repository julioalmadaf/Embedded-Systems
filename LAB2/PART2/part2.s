/* Program that counts consecutive 1’s */ 
			.text 
			.global 	_start 
_start: 
			LDR 		R1, TEST_NUM 	// load the data word into R1

			MOV			R5, #0		//Initialize R5 with a 0, it is the one that is going to hold the largest string of 1s among all the TEST_NUM
			LDR			R3, =TEST_NUM	//holds test_num location
LOOP:		MOV 		R0, #0 			// R0 will hold the result 
			MOV			R4,R3			//copy direction to r4
			CMP			R1, #0			//If R1 holds 0, end
			BEQ			END				
			BL			ONES			
			ADD			R3, R3, #4		//obtain the next VALUE
			LDR			R4, [R3]		//load the new value to R4
			MOV			R1, R4			//COPY IT TO R1
			CMP			R0, R5			//CHECK WHICH HAS LARGEST STRINGS OF 1S
			BLE			LOOP
			MOV			R5, R0			//UPDATE LARGEST STRING OF 1S
			B			LOOP
			
END: 		B 			END		
			
ONES: 		CMP 		R1, #0 			// loop until the data contains no more 1’s 
			BEQ 		DONE 
			LSR 		R2, R1, #1 		// perform SHIFT, followed by AND 
			AND 		R1, R1, R2 
			ADD		 	R0, #1 			// count the string lengths so far 
			B 			ONES
DONE:		BX			LR


TEST_NUM: 	.word 		0x103fe00f, 0x11111111, 0xFFFFFFFF, 0xABCDEFFE
			.word		0X10203040, 0X12345678, 0X9ABCDEF0,	0X00000001
			.word		0XFEDCBA12, 0X00000000
			
			.end