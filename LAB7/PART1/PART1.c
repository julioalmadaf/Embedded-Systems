#include <stdio.h>

/******************************************************************** * 
Pushbutton - Interrupt Service Routine * * This routine checks which KEY has been pressed. 
It writes to HEX0 *******************************************************************/
int f0=0, f1=0, f2=0, f3=0;

void pushbutton_ISR( void ) 
{
	/* KEY base address */ 
	volatile int *KEY_ptr = (int *) 0xFF200050;
	/* HEX display base address */ 
	volatile int *HEX3_HEX0_ptr = (int *) 0xFF200020; 
	int press, HEX_bits;
	press = *(KEY_ptr + 3);
	// read the pushbutton interrupt register 
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if (press & 0x1) // KEY0 
		if(f0==0)
		{
			f0=1;
			f1=0;
			f2=0;
			f3=0;
			HEX_bits = 0b00111111; 
		}
		else
		{
			HEX_bits = 0b00000000; 
			f0=0;
		}
		
	else if (press & 0x2) // KEY1 
	{
		if(f1==0)
		{
			f0=0;
			f1=1;
			f2=0;
			f3=0;
			HEX_bits = 0b0000011000000000; 
		}
		else
		{
			HEX_bits = 0b00000000; 
			f1=0;
		}
	}
	else if (press & 0x4) // KEY2 
	{
		if(f2==0)
		{
			f0=0;
			f1=0;
			f2=1;
			f3=0;
			HEX_bits = 0b010110110000000000000000; 
		}
		else
		{
			HEX_bits = 0b00000000; 
			f2=0;
		}
	}
	else // press & 0x8, which is KEY3 
	{
		if(f3==0)
		{
			f0=0;
			f1=0;
			f2=0;
			f3=1;
			HEX_bits = 0b01001111000000000000000000000000;
		}
		else
		{
			HEX_bits = 0b00000000; 
			f3=0;
		}
	}
	*HEX3_HEX0_ptr = HEX_bits; 
	return;
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

// Define the IRQ exception handler 
void __attribute__ ((interrupt)) __cs3_isr_irq (void) 
{ 
// Read the ICCIAR from the CPU Interface in the GIC 
	int interrupt_ID = *((int *) 0xFFFEC10C);
	if (interrupt_ID == 73) // check if interrupt is from the KEYS 
		pushbutton_ISR (); 
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
	set_A9_IRQ_stack();				//initialize  stack pointer
	config_GIC();					//configure the general interrupt controller
	config_KEYS();					//configure pushbutton KEYS to generate interrupts
	
	enable_A9_interrupts();			// enable interrupts in the A9 processor
	
	while(1);						//wait for an interrupt
}



