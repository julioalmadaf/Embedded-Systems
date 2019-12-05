#include <stdio.h>

/******************************************************************** * 
Pushbutton - Interrupt Service Routine * * This routine checks which KEY has been pressed. 
It writes to HEX0 *******************************************************************/
int count=0;
int run =1;
volatile int * HPS_timer_ptr = (int *) 0xFFC08000; 	// Timer base address
volatile int* seg1= (int*) 0xFF200020;
volatile int* seg2 = (int*) 0xFF200030;
volatile int * A9P_timer_ptr = (int *) 0xFFFEC600; 	// Timer base address

int counter = 25000000;						//period=1/(100MHZ)x(100 x 10^6)=1 sec

int h1, h2, h3, h4, h5, h6;				//FOR EACH HEX 
int d1, d2, s1, s2, m1, m2; 			//MM:SS:DD
int M,S,D;

int counterA9P=2000000;
int counterReloj=0;
int flagKey =0;

int swnum(int num)						//CONVERT TO HEX VALUES
{
	int hex;
	switch(num)
	{
		case 0:
			hex=0b00111111;
			break;
		case 1:
			hex=0b00000110;
			break;
		case 2:
			hex=0b01011011;
			break;
		case 3:
			hex=0b01001111;
			break;
		case 4:
			hex=0b01100110;
			break;
		case 5:
			hex=0b01101101;
			break;
		case 6:
			hex=0b01111101;
			break;
		case 7:
			hex=0b00000111;
			break;
		case 8:
			hex=0b01111111;
			break;
		case 9:
			hex=0b01100111;
			break;
		default:
			break;
	}
	return hex;
}

void div(int num)						//GET EACH VALUE FOR EACH HEX
{
	m2=num/100000;
	d1=num%10;
	num-=d1;
	d2=num%100;
	num-=d2;
	d2=d2/10;
	s1=num%1000;
	num-=s1;
	s1=s1/100;
	s2=num%10000;
	num-=s2;
	s2=s2/1000;
	m1=num%100000;
	num-=m1;
	m1=m1/10000;
	
	h1=swnum(d1);
	h2=swnum(d2);
	h3=swnum(s1);
	h4=swnum(s2);
	h5=swnum(m1);
	h6=swnum(m2);
	
	M=(h5|(h6<<8));
	S=(h3|(h4<<8));
	D=(h1|(h2<<8));
}

void printHex()
{
	*(seg1) = ((S<<16)|D);
	*(seg2) = M;
}

//setup HPS timer
void config_HPS_timer()
{
	*(HPS_timer_ptr + 0x2) = 0; 						//write to control register to stop timer
	//set the timer period
	*(HPS_timer_ptr)=counter;						//write to timer load register
	//write to control register to start timer, with interrupts
	*(HPS_timer_ptr + 2) = 0b011;						//int mask =0, mode = 1, enable =1
}

void A9Ptmr_ISR()
{
	counterReloj++;
	if(counterReloj == 599999) counterReloj=0;
	div(counterReloj);
	*(A9P_timer_ptr+3)=1;
}

void pushbutton_ISR( void ) 
{
	// KEY base address  
	volatile int *KEY_ptr = (int *) 0xFF200050; 
	int press;
	press = *(KEY_ptr + 3);
	// read the pushbutton interrupt register 
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if (press & 0x1) // KEY0 
	{
		//config_HPS_timer();
		if(run==1)
		{
			run=0;
		}
		else
		{
			run=1;
		}
	}	
	else if (press & 0x2) // KEY1 
	{
		*(HPS_timer_ptr + 0x2) = 0; 						//write to control register to stop timer
		//set the timer period
		counter/=2;
		*(HPS_timer_ptr)=counter;							//write to timer load register
		*(HPS_timer_ptr + 2) = 0b011;						//int mask =0, mode = 1, enable =1
	}
	else if (press & 0x4) // KEY2 
	{
		*(HPS_timer_ptr + 0x2) = 0; 						//write to control register to stop timer
		//set the timer period
		counter*=2;
		*(HPS_timer_ptr)=counter;							//write to timer load register
		*(HPS_timer_ptr + 2) = 0b011;						//int mask =0, mode = 1, enable =1
	}
	else // press & 0x8, which is KEY3 
	{
		if(flagKey==0)
		{
			*(A9P_timer_ptr + 2) = 0b101;						//int mask =0, mode = 1, enable =1
			flagKey=1;
		}
		else
		{
			*(A9P_timer_ptr + 2) = 0b111;						//int mask =0, mode = 1, enable =1
			flagKey=0;
		}
	}
}

//
void HPS_timer_ISR()
{
	volatile int * HPS_timer_ptr= (int *)0xFFC08000;	//HPS timer address
	count+=run;											//used by main program
	*(HPS_timer_ptr+3); 							//read timer end of interrupt register to clear interrupt
}

//initialize the banked stack pointer register for IRQ mode
void set_A9_IRQ_stack(void)
{
	int stack, mode;
	stack=0xFFFFFFFF - 7; 							// Top of A9 onchip memory, aligned to 8 bytes
	mode= 0b11010010; 								//change processor to IRQ mode with interrupts disabled
	asm ("msr cpsr, %[ps]"::[ps] "r" (mode));
	asm	("mov sp, %[ps]"::[ps]"r"(stack));			// set banked stack pointer
	mode=0b11010011;								//go back to svc mode before executing subroutine return
	asm("msr cpsr, %[ps]"::[ps]"r"(mode));			
}	

/* * Configure registers in the GIC for an individual Interrupt ID. 
We * configure only the Interrupt Set Enable Registers (ICDISERn) and 
* Interrupt Processor Target Registers (ICDIPTRn). The default (reset) 
* values are used for other registers in the GIC */
void config_interrupt( int N, int CPU_target)
{
	int reg_offset, index, value, address;
	//configure the interrup set-enable registers(ICDISERn)
	//reg_offset= (integer_dive(N/32)*4; value=1<<(N mod 32)
	reg_offset= (N>>3) & 0xFFFFFFFC;
	index= N & 0x1F;
	value=0x1 << index;
	address = 0xFFFED100 + reg_offset;
	//using the adress and valie, set the appropiate bit
	*(int *)address |= value;
	
	//configure the interrupt processor targets register (ICDIPTRn)
	//reg_offset = integer_div(N/4)*4; index =N mod 4
	reg_offset = (N & 0xFFFFFFFC);
	index=N & 0x3;
	address= 0xFFFED800 +reg_offset +index;
	//using the address and value, write to (only) the appropiate byte
	*(char *)address = (char) CPU_target;
}

//configure the Generic Interrupt controller
void config_GIC()
{
	config_interrupt(29,1);						//configure the HPS0 timer interrupt (199)
	config_interrupt(73,1);							//configure the FPGA keys interrupt (73)
	config_interrupt(199,1);						//configure the HPS0 timer interrupt (199)
	
	
	*((int *) 0xFFFEC104) = 0xFFFF;					//SET INTERRUPT PRIORITY MASK REGISTER (ICCPMR). ENABLE AL PRIORITIES
	*((int *) 0xFFFEC100) = 1;						//SET THE ENABLE IN THE CPU INTERFACE CONTROL REGISTER (ICCICR)
	*((int *) 0xFFFED000) =1;						//SET THE ENABLE IN THE DISTRIBUTOR CONTROL REGISTER (ICDDCR)	
}

//SET THE PUSHBUTTON keys port in the FPGA
void config_KEYS()
{
	volatile int * KEY_PTR= (int *) 0xFF200050;		//KEY BASE ADDRESS
	*(KEY_PTR + 2) = 0xF;							//ENABLE INTERRUPTS FOR ALL FOUR KEYS
}

void config_A9P_timer()
{
	*(A9P_timer_ptr)=counterA9P;						//write to timer load register
	//write to control register to start timer, with interrupts
	*(A9P_timer_ptr + 2) = 0b111;						//int mask =0, mode = 1, enable =1
}

//turn on interrupts in the ARM processor
void enable_A9_interrupts()
{
	int status= 0b01010011;
	asm("msr cpsr, %[ps]"::[ps] "r"(status));
}

// Define the IRQ exception handler 
void __attribute__ ((interrupt)) __cs3_isr_irq (void) 
{ 
// Read the ICCIAR from the CPU Interface in the GIC 
	int interrupt_ID = *((int *) 0xFFFEC10C);
	if(interrupt_ID == 29)
	{
		A9Ptmr_ISR();
	}
	else if (interrupt_ID == 73) 							// check if interrupt is from the KEYS 
		pushbutton_ISR (); 
	else if(interrupt_ID == 199)						// check if interrupt is from the HPS0 timer 
		HPS_timer_ISR();
	else 
		while (1); // if unexpected, then stay here
// Write to the End of Interrupt Register (ICCEOIR) 
	*((int *) 0xFFFEC110) = interrupt_ID;
}
// Define the remaining exception handlers 

void __attribute__ ((interrupt)) __cs3_reset (void) 
{ 
	while(1); 
}

void __attribute__ ((interrupt)) __cs3_isr_undef (void) 
{ 
	while(1); 
}

void __attribute__ ((interrupt)) __cs3_isr_swi (void) 
{ 
	while(1); 
}

void __attribute__ ((interrupt)) __cs3_isr_pabort (void) 
{ 
	while(1); 
}

void __attribute__ ((interrupt)) __cs3_isr_dabort (void) 
{ 
	while(1); 
}

void __attribute__ ((interrupt)) __cs3_isr_fiq (void) 
{ 
	while(1); 
}

int main(void)
{
	volatile int *LEDR_PTR=(int *)0xFF200000;
	
	set_A9_IRQ_stack();				//initialize  stack pointer
	config_GIC();					//configure the general interrupt controller
	config_HPS_timer();
	config_KEYS();					//configure pushbutton KEYS to generate interrupts
	config_A9P_timer();
	
	enable_A9_interrupts();			// enable interrupts in the A9 processor
	
	while(1)						//wait for an interrupt
	{
		*(LEDR_PTR)=count;
		printHex();
	}
}



