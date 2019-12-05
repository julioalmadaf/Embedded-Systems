#include "address_map_arm.h"
#include "lcd_driver.h"
#include "lcd_graphic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

volatile int * SW = (int *)SW_BASE;				//SWITCH REGISTER
volatile int *LEDR_PTR = (int *)0xFF200000;

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_MPcore_private_timer(void);
void config_PS2(void);
void enable_A9_interrupts(void);

int julio;
char *lcd1;
char *lcd2;
char *lcd3;

/* these globals are written by interrupt service routines; we declare them as
 * volatile to avoid the compiler caching their values, even in registers */
extern volatile char byte1, byte2, byte3; /* modified by PS/2 interrupt service routine */
extern volatile int timeout; // used to synchronize with the timer

/* function prototypes */
void HEX_PS2(char, char, char);


/***************************************************************************************
 * Pushbutton - Interrupt Service Routine                                
 *                                                                          
 * This routine checks which KEY has been pressed. If it is KEY1 or KEY2, it writes this 
 * value to the global variable key_pressed. If it is KEY3 then it loads the SW switch 
 * values and stores in the variable pattern
****************************************************************************************/
void PS2_ISR( void )
{
  	volatile int * PS2_ptr = (int *) 0xFF200100;		// PS/2 port address
	int PS2_data, RAVAIL;



	PS2_data = *(PS2_ptr);									// read the Data register in the PS/2 port
	RAVAIL = (PS2_data & 0xFFFF0000) >> 16;			// extract the RAVAIL field
	if (RAVAIL > 0)
	{
		/* always save the last three bytes received */
		byte1 = byte2;
		byte2 = byte3;
		byte3 = PS2_data & 0xFF;
	}
    
	return;
}

void debugButton(void)
{
    clear_screen();

    if(byte1==0x0E)
    {
        LCD_text("Byte1=0E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=0E",2);
    } 
    if(byte1==0x16){
        LCD_text("Byte1=16",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=16",2);
    }
    if(byte1==0x1E){
        LCD_text("Byte1=1E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1E",2);
    }
    if(byte1==0x26){
        LCD_text("Byte1=26",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=26",2);
    }
    if(byte1==0x25){
        LCD_text("Byte1=25",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=25",2);
    }
    if(byte1==0x2E){
        LCD_text("Byte1=2E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2E",2);
    }
    if(byte1==0x36){
        LCD_text("Byte1=36",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=36",2);
    }
    if(byte1==0x3D){
        LCD_text("Byte1=3D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=3D",2);
    }
    if(byte1==0x3E){
        LCD_text("Byte1=3E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=3E",2);
    }
    if(byte1==0x46){
        LCD_text("Byte1=46",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=46",2);
    }
    if(byte1==0x45){
        LCD_text("Byte1=45",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=45",2);
    }
    if(byte1==0x4E){
        LCD_text("Byte1=4E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4E",2);
    }
    if(byte1==0x55){
        LCD_text("Byte1=55",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=55",2);
    }
    if(byte1==0x66){
        LCD_text("Byte1=66",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=66",2);
    } 
    if(byte1==0x15){
        LCD_text("Byte1=15",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=15",2);
    }
    if(byte1==0x1D)
    {
        LCD_text("Byte1=1D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1D",2);
    }
    if(byte1==0x24){
        LCD_text("Byte1=24",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=24",2);
    }
    if(byte1==0x2D)
    {
        LCD_text("Byte1=2D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2D",2);
    }
    if(byte1==0x2C)
    {
        LCD_text("Byte1=2C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2C",2);
    }
    if(byte1==0x35)
    {
        LCD_text("Byte1=35",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=35",2);
    }
    if(byte1==0x3C)
    {
        LCD_text("Byte1=3C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=3C",2);
    }
    if(byte1==0x43)
    {
        LCD_text("Byte1=43",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=43",2);
    }
    if(byte1==0x44)
    {
        LCD_text("Byte1=44",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=44",2);
    }
    if(byte1==0x4D)
    {
        LCD_text("Byte1=4D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4D",2);
    }
    if(byte1==0x54)
    {
        LCD_text("Byte1=54",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=54",2);
    }
    if(byte1==0x5B)
    {
        LCD_text("Byte1=5B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=5B",2);
    }
    if(byte1==0x58)
    {
        LCD_text("Byte1=58",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=58",2);
    } 
    if(byte1==0x1C)
    {
        LCD_text("Byte1=1C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1C",2);
    }
    if(byte1==0x1B)
    {
        LCD_text("Byte1=1B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1B",2);
    }
    if(byte1==0x23)
    {
        LCD_text("Byte1=23",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=23",2);
    }
    if(byte1==0x2B)
    {
        LCD_text("Byte1=2B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2B",2);
    }
    if(byte1==0x34)
    {
        LCD_text("Byte1=34",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=34",2);
    }
    if(byte1==0x33)
    {
        LCD_text("Byte1=33",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=33",2);
    }
    if(byte1==0x3B)
    {
        LCD_text("Byte1=3B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=3B",2);
    }
    if(byte1==0x42)
    {
        LCD_text("Byte1=42",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=42",2);
    }
    if(byte1==0x4B)
    {
        LCD_text("Byte1=4B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4B",2);
    }
    if(byte1==0x5A)
    {
        LCD_text("Byte1=5A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=5A",2);
    }
    if(byte1==0x1A)
    {
        LCD_text("Byte1=1A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1A",2);
    }
    if(byte1==0x22)
    {
        LCD_text("Byte1=22",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=22",2);
    }
    if(byte1==0x21)
    {
        LCD_text("Byte1=21",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=21",2);
    }
    if(byte1==0x2A)
    {
        LCD_text("Byte1=2A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2A",2);
    }
    if(byte1==0x32)
    {
        LCD_text("Byte1=32",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=32",2);
    }
    if(byte1==0x31)
    {
        LCD_text("Byte1=31",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=31",2);
    }
    if(byte1==0x3A)
    {
        LCD_text("Byte1=3A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=3A",2);
    }
    if(byte1==0x41)
    {
        LCD_text("Byte1=41",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=41",2);
    }
    if(byte1==0x49)
    {
        LCD_text("Byte1=49",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=49",2);
    }
    if(byte1==0x4A)
    {
        LCD_text("Byte1=4A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4A",2);
    }
    if(byte1==0x29)
    {
        LCD_text("Byte1=29",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=29",2);
    }
    if(byte1==0x0D)
    {
        LCD_text("Byte1=0D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=0D",2);
    }
    if(byte1==0x4C)
    {
        LCD_text("Byte1=4C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4C",2);
    }if(byte1==0x52)
    {
        LCD_text("Byte1=52",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=52",2);
    }if(byte1==0x5D)
    {
        LCD_text("Byte1=5D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=5D",2);
    }
    if(byte1==0x12)
    {
        LCD_text("Byte1=12",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=12",2);
    }
    if(byte1==0x61)
    {
        LCD_text("Byte1=61",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=61",2);
    }
    if(byte1==0x29)
    {
        LCD_text("Byte1=29",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=29",2);
    }
    if(byte1==0x59)
    {
        LCD_text("Byte1=59",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=59",2);
    }
    if(byte1==0x14)
    {
        LCD_text("Byte1=14",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=14",2);
    }
    if(byte1==0x11)
    {
        LCD_text("Byte1=11",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=11",2);
    }
    if(byte1==0x11)
    {
        LCD_text("Byte1=11",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=11",2);
    }
    if(byte1==0x76)
    {
        LCD_text("Byte1=76",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=76",2);
    }
    if(byte1==0x05)
    {
        LCD_text("Byte1=05",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=05",2);
    }
    if(byte1==0x06)
    {
        LCD_text("Byte1=06",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=06",2);
    }
    if(byte1==0x05)
    {
        LCD_text("Byte1=05",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=05",2);
    }
    if(byte1==0x04)
    {
        LCD_text("Byte1=04",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=04",2);
    }
    if(byte1==0x0C)
    {
        LCD_text("Byte1=0C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=0C",2);
    }
    if(byte1==0x03)
    {
        LCD_text("Byte1=03",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=03",2);
    }
    if(byte1==0x0B)
    {
        LCD_text("Byte1=0B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=0B",2);
    }
    if(byte1==0x83)
    {
        LCD_text("Byte1=83",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=83",2);
    }
    if(byte1==0x0A)
    {
        LCD_text("Byte1=0A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=0A",2);
    }
    if(byte1==0x01)
    {
        LCD_text("Byte1=01",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=01",2);
    }
    if(byte1==0x09)
    {
        LCD_text("Byte1=09",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=09",2);
    }
    if(byte1==0x78)
    {
        LCD_text("Byte1=78",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=78",2);
    }
    if(byte1==0x07)
    {
        LCD_text("Byte1=07",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=07",2);
    }
    if(byte1==0x7E)
    {
        LCD_text("Byte1=7E",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7E",2);
    }
    if(byte1==0x77)
    {
        LCD_text("Byte1=77",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=77",2);
    }
    if(byte1==0x7C)
    {
        LCD_text("Byte1=7C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7C",2);
    }
    if(byte1==0x7B)
    {
        LCD_text("Byte1=7B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7B",2);
    }
    if(byte1==0x6C)
    {
        LCD_text("Byte1=6C",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=6C",2);
    }
    if(byte1==0x75)
    {
        LCD_text("Byte1=75",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=75",2);
    }
    if(byte1==0x7D)
    {
        LCD_text("Byte1=7D",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7D",2);
    }
    if(byte1==0x6B)
    {
        LCD_text("Byte1=6B",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=6B",2);
    }
    if(byte1==0x73)
    {
        LCD_text("Byte1=73",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=73",2);
    }
    if(byte1==0x74)
    {
        LCD_text("Byte1=74",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=74",2);
    }
    if(byte1==0x69)
    {
        LCD_text("Byte1=69",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=69",2);
    }
    if(byte1==0x72)
    {
        LCD_text("Byte1=72",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=72",2);
    }
    if(byte1==0x7A)
    {
        LCD_text("Byte1=7A",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7A",2);
    }
    if(byte1==0x70)
    {
        LCD_text("Byte1=70",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=70",2);
    }
    if(byte1==0x71)
    {
        LCD_text("Byte1=71",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=71",2);
    }
    if(byte1==0x79)
    {
        LCD_text("Byte1=79",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=79",2);
    }
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x5A))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=5A",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x4A))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=4A",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x70))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=70",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x6C))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=6C",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x7D))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7D",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x71))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=71",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x69))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=69",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x7A))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=7A",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x12))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=12",2);	
	}
    if((byte1==0x14)&&(byte2==0xF0)&&(byte3==0x77))
	{
		LCD_text("Byte1=14",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=77",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x14))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=14",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x2F))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=2F",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x27))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=27",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x11))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=11",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x1F))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=1F",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x74))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=74",2);	
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x72))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=72",2);
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x75))
	{
		LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=75",2);
	}
    if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x6B))
    {
        LCD_text("Byte1=E0",0);
        LCD_text("Byte2=F0",1);
        LCD_text("Byte3=6B",2);
    }
    refresh_buffer();
}

/********************************************************************************
 * This program performs the following:
 * 	   Displays the last three bytes of data received from the PS/2 port
 * 	   on the HEX displays on the board. The PS/2 port is handled using
 * 	   interrupts
********************************************************************************/
int main(void) {
    /* initialize some variables */
    byte1        = 0;
    byte2        = 0;
    byte3        = 0; // used to hold PS/2 data
    timeout      = 0; // synchronize with the timer

    volatile int delay_count; // volatile so C compiler doesn't remove the loop

    set_A9_IRQ_stack();            // initialize the stack pointer for IRQ mode
    config_GIC();                  // configure the general interrupt controller
    config_MPcore_private_timer(); // configure ARM A9 private timer
    config_PS2();  // configure PS/2 port to generate interrupts
    init_spim0();
    init_lcd();

    enable_A9_interrupts(); // enable interrupts

    //Limpia la pantalla
    clear_screen();

    while (1) 
    {
        //clear_screen();

        while (!timeout); // wait to synchronize with timer

        /* display PS/2 data (from interrupt service routine) on HEX displays */
        HEX_PS2(byte1, byte2, byte3); 

        debugButton();

        timeout = 0;
    }
}

/* setup private timer in the ARM A9 */
void config_MPcore_private_timer() {
    volatile int * MPcore_private_timer_ptr = (int *)MPCORE_PRIV_TIMER; // timer base address

    /* set the timer period */
    int counter = 20000000; // period = 1/(200 MHz) x 40x10^6 = 0.2 sec
    *(MPcore_private_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(MPcore_private_timer_ptr + 2) = 0x7; // int mask = 1, mode = 1, enable = 1
}

/* setup the PS/2 interrupts */
void config_PS2() {
    volatile int * PS2_ptr = (int *)PS2_BASE; // PS/2 port address

    *(PS2_ptr) = 0xFF; /* reset */
    *(PS2_ptr + 1) = 0x1; /* write to the PS/2 Control register to enable interrupts */
}

/****************************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
****************************************************************************************/
void HEX_PS2(char b1, char b2, char b3) {
    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display in the DE2 Media Computer, for the hex digits 0 -
     * F */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int  shift_buffer, nibble;
    unsigned char code;
    int           i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;

    for (i = 0; i < 6; ++i) 
    {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code   = seven_seg_decode_table[nibble];
        hex_segs[i]  = code;
        shift_buffer = shift_buffer >> 4;
    }

    /* drive the hex displays */
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}