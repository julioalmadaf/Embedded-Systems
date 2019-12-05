#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "address_map_arm.h"

#define STANDARD_X 320
#define STANDARD_Y 240

volatile int pixel_buffer_start;					//global variable

volatile int *  BUFFER = (int *) PIXEL_BUF_CTRL_BASE;					//BUFFER REGISTER
volatile int *  BBUFFER = (int *) (PIXEL_BUF_CTRL_BASE + 0x4);			//BACKBUFFER REGISTER
volatile int * RESOLUTION = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);			//RESOLUTION REGISTER
volatile int * STATUS = (int *)(PIXEL_BUF_CTRL_BASE + 0xC);				//RESOLUTION REGISTER
volatile int * SW = (int *)SW_BASE;				//RESOLUTION REGISTER

int n=0;

int x[255];
int y[255];

int length=5;
	
void VSYNC()
{
	*(BUFFER) = 0x1;														//SWAP BETWEEN BUFFERS HAPPEN
	while(*(STATUS)==0x1);
	*(BUFFER) = 0x0;
}

void swap(int *a, int *b)
{
   int t;
 
   t  = *b;
   *b = *a;
   *a = t;
}

void plot_pixel(int x, int y, short int line_color)
{
	*(short int*)(pixel_buffer_start + (y<<10)+(x<<1))=line_color;
}

int y_step=0;
void draw_line(int x0, int x1, int y0, int y1, short int color)
{
	bool is_steep= abs(y1-y0) > abs(x1-x0);						//THE LINE DOESNT MOVE THAT MUCH ON THE HORIZONTAL AXIS
	if(is_steep)												//				
	{
		swap(&x0,&y0);
		swap(&x1,&y1);
	}
	if( x0 > x1)
	{
		swap(&x0,&x1);
		swap(&y0,&y1);
	}
	
	int deltax= x1-x0;
	int deltay= y1-y0;
	int error = -(deltax/2);
	int y = y0;
	if(y0 < y1) y_step=1;
	else y_step=-1;
	int x=0;
	for(x=x0;x<x1;x++)
	{
		if (is_steep) plot_pixel(y,x,color);
		else plot_pixel(x,y,color);
		error += abs(deltay);
		if (error>=0)
		{
			y += y_step;
			error -= deltax;
		}
	}
}

void CLEAR() {
    int pixel_ptr, row, col;
    for (row = 0; row <= STANDARD_Y; row++)
        for (col = 0; col <= STANDARD_X; ++col) {
            pixel_ptr = pixel_buffer_start + (row << 10) + (col << 1);
            *(short *)pixel_ptr = 0; // set pixel color
        }
}

int valueY =0;
int flag =0;

void moveLine()
{
	draw_line(0,319,valueY,valueY,0x001F);										//THIS LINE IS BLUE
	if(flag==0)
	{
		valueY++;
		if(valueY == 239) flag =1;
	}
	else
	{
		valueY--;
		if(valueY == 0) flag=0;
	}
}

void VGA_rect(int x1, int y1, int width, short int color)
{
    int x2 = x1 + width;
    int y2 = y1 + width;
    int i;
    for (; y1 <= y2; y1++)
	{
		draw_line(x1, x2, y1,  y1, color);
	}
}

	
//ROUTINE FOR WHEN A BUTTON IS PRESSED
void pushbutton_ISR( void ) 			
{
	/* KEY base address */ 
	volatile int *KEY_ptr = (int *) 0xFF200050; 
	int press;
	int i;
	/* create a message to be displayed on the VGA display */
	press = *(KEY_ptr + 3);
	// read the pushbutton interrupt register 
	*(KEY_ptr + 3) = press; 									// Clear the interrupt
	CLEAR();
	if (press & 0x1) 											// KEY0 
	{
		
	}	
	else if (press & 0x2) 										// KEY1 
	{
		
	}
	else if (press & 0x4) 										// KEY2 
	{
		n++;
		for(i=1;i<=n;i++) 
		{
			int j = i;
			j--;
			if (i==n) VGA_rect( x[i] = rand() % 319, y[i] = rand() %239, length, 0x07E0);
			else VGA_rect( x[i], y[i], length, 0x07E0);
			if( *SW != 0)
			{
				if (i>=2) 
				{
					draw_line(x[j],x[i],y[j],y[i],0x07E0);
				}
				if(i==n) draw_line(x[i],x[1],y[i],y[1],0x07E0);
			}
		}
	}
	else  														// KEY3
	{
		n--;
		if(n<=0)
		{			
			n=0;
			CLEAR();
		}
		
		for(i=1;i<=n;i++) 
		{
			int j = i;
			j--;
			VGA_rect( x[i], y[i], length, 0x07E0);
			if( *SW != 0)
			{
				if (i>=2) 
				{
					draw_line(x[j],x[i],y[j],y[i],0x07E0);
				}
				if(i==n) draw_line(x[i],x[1],y[i],y[1],0x07E0);
			}
		}
	}
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
	if (interrupt_ID == 73) 							// check if interrupt is from the KEYS 
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
	set_A9_IRQ_stack();										//initialize  stack pointer
	config_GIC();											//configure the general interrupt controller
	config_KEYS();											//configure pushbutton KEYS to generate interrupts
	
	enable_A9_interrupts();									// enable interrupts in the A9 processor

	
	pixel_buffer_start= *BUFFER;
	
	CLEAR();
	
	while(1)
	{
		
	}
}
