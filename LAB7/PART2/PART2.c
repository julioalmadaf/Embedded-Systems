#include <stdio.h>

/******************************************************************** * 
Pushbutton - Interrupt Service Routine * * This routine checks which KEY has been pressed. 
It writes to HEX0 *******************************************************************/
int count=0;
int run =1;

void pushbutton_ISR( void ) 
{
	/* KEY base address */ 
	volatile int *KEY_ptr = (int *) 0xFF200050; 
	int press;
	press = *(KEY_ptr + 3);
	// read the pushbutton interrupt register 
	*(KEY_ptr + 3) = press; // Clear the interrupt
		if(run==1)
		{
			run=0;
		}
		else
		{
			run=1;
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

//turn on interrupts in the ARM processor
void enable_A9_interrupts()
{
	int status= 0b01010011;
	asm("msr cpsr, %[ps]"::[ps] "r"(status));
}

//setup HPS timer
void config_HPS_timer()
{
	volatile int * HPS_timer_ptr = (int *) 0xFFC08000; 	// Timer base address
	*(HPS_timer_ptr + 0x2) = 0; 						//write to control register to stop timer
	//set the timer period
	int counter = 25000000; 							//period=1/(100MHZ)x(100 x 10^6)=1 sec
	*(HPS_timer_ptr)=counter;						//write to timer load register
	
	//write to control register to start timer, with interrupts
	*(HPS_timer_ptr + 2) = 0b011;						//int mask =0, mode = 1, enable =1
}

// Define the IRQ exception handler 
void __attribute__ ((interrupt)) __cs3_isr_irq (void) 
{ 
// Read the ICCIAR from the CPU Interface in the GIC 
	int interrupt_ID = *((int *) 0xFFFEC10C);
	if (interrupt_ID == 73) 							// check if interrupt is from the KEYS 
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
	
	enable_A9_interrupts();			// enable interrupts in the A9 processor
	
	while(1)						//wait for an interrupt
	{
		*(LEDR_PTR)=count;
	}
}



