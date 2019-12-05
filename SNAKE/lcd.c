#include "lcd_driver.h"
#include "lcd_graphic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "address_map_arm.h"


volatile int * SW = (int *)SW_BASE;				//SWITCH REGISTER
volatile int *LEDR_PTR = (int *)0xFF200000;

int score;
int flagClear=0;

int x, y, length, dir_x=1, dir_y=1;
int flagx, flagy;
int flaglose;
int flagCollision=0;
int randx = 0;
int randy = 0;
int snakeLength;
int snakeBodyX[255];
int snakeBodyY[255];
int toogleX=1;
int toogleY=1;

/* create a message to be displayed on the VGA display */
//Menu principal
char text_top_lcd[17]    = "  ANY SWITCH TO \0";
char text_bottom_lcd[17] = "      START     \0";
//Pantalla de perdedor
char loser[17] = "      LOSER     \0";

//ROUTINE FOR WHEN A BUTTON IS PRESSED
void pushbutton_ISR( void ) 			
{
	/* KEY base address */ 
	volatile int *KEY_ptr = (int *) 0xFF200050; 
	int press;

	/* create a message to be displayed on the VGA display */
	press = *(KEY_ptr + 3);

	// read the pushbutton interrupt register 
	*(KEY_ptr + 3) = press; 									// Clear the interrupt

    // KEY0 
	if (press & 0x1) 											
	{
        //Mover punto a la derecha
        if (toogleX==1)
        {
            dir_x = 1;
            flagx=1;
            flagy=0;
            toogleX=0;
        }	
        toogleY=1;
	}
    // KEY1 
	else if (press & 0x2) 										
	{
        //Mover punto para abajo
        if(toogleY==1)
        {
            dir_y = 1;
            flagx=0;
            flagy=1;
            toogleY=0;
        }
        toogleX=1;
	}
    // KEY2 
	else if (press & 0x4) 										
	{
        //Mover punto para arriba
        if(toogleY==1)
        {
            dir_y = -1;
            flagx=0;
            flagy=1;
            toogleY=0;
        }
        toogleX=1;
    }
    //KEY3
    else                                                        
    {
        //Mover punto a la izquierda
        if(toogleX==1)
        {
            dir_x = -1;
            flagx=1;
            flagy=0;
            toogleX=0;
        }
        toogleY=1;
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

//SCORE IN BINARY, ON THE LEDS
void SCORE()
{   
    *(LEDR_PTR)=++score;
    LCD_rect(randx, randy, length, 0);
    randx = rand()%122;
    randy = rand()%58;
}

//CHECK SW0 TO START GAME
void START()
{
    //Reinicia la partida
    if(*SW == 0)
    {
        //limpia la pantalla
        if(flagClear==1) clear_screen();
        //
        if(flaglose==1) flaglose=0;
        flagClear=0;
        //Imprime el mensaje del menu principal
        LCD_text(text_top_lcd, 0);
        LCD_text(text_bottom_lcd, 1);
        //Reinicia el puntaje
        score=0;
        //Borra los valores guardados en los leds
        *(LEDR_PTR)=0;
        snakeLength=5;
        /* initialize first position of box */
        snakeBodyX[0]=64;
        snakeBodyY[0]=32;
        snakeBodyX[1]=63;
        snakeBodyY[1]=32;
        snakeBodyX[2]=62;
        snakeBodyY[2]=32;
        snakeBodyX[3]=61;
        snakeBodyY[3]=32;
        snakeBodyX[4]=60;
        snakeBodyY[4]=32;
        length = 3;
        dir_x = 1;
        dir_y = 1;
        flagx = 1;
        flagy = 0;
        randx = rand()%122;
        randy = rand()%58;
        flagCollision=0;
    }
    else
    {
        //Borra el mensaje del menu principal
        if(flagClear==0) clear_screen();
        flagClear=1;
        //Aumenta el score
        if(((snakeBodyX[0]==randx)&&(snakeBodyY[0]==randy))||((snakeBodyX[0]==randx++)&&(snakeBodyY[0]==randy++))||((snakeBodyX[0]==randx--)&&(snakeBodyY[0]==randy--))||((snakeBodyX[0]==randx++)&&(snakeBodyY[0]==randy--))||((snakeBodyX[0]==randx--)&&(snakeBodyY[0]==randy++)))
        {
            SCORE();
            snakeLength++;
        }
        /* erase last box */
        //LCD_rect(snakeBodyX[0], snakeBodyY[0], length, 0);
        LCD_rect(snakeBodyX[snakeLength], snakeBodyY[snakeLength], length, 0);

        int i,j;
        int SLtemp = snakeLength;
        SLtemp-=1;
        for(i=snakeLength, j=SLtemp; i>0 ; i--, j--)
        {
            snakeBodyX[i]=snakeBodyX[j];
            snakeBodyY[i]=snakeBodyY[j];
        }

        if (flaglose==0)
        {
            //Imprime el cuadro en una nueva posicion
            if(flagx) snakeBodyX[0] += dir_x;
            if(flagy) snakeBodyY[0] += dir_y;
        }
        
        //Dibuja el cuadro
        int k;
        for(k=0; k<=snakeLength-1;k++)
        {
            LCD_rect(snakeBodyX[k], snakeBodyY[k], length, 1);
        }

        int y;
        for(y=1;y<=snakeLength-1;y++)
        {
            if((snakeBodyX[0]==snakeBodyX[y])&&(snakeBodyY[0]==snakeBodyY[y])) flagCollision=1;
        }

        //En realidad pierde la partida
        if ((snakeBodyX[0] + length >= SCREEN_WIDTH - 1 && dir_x == 1) || (snakeBodyX[0] <= 0 && dir_x == -1) || (snakeBodyY[0] + length >= SCREEN_HEIGHT - 1 && dir_y == 1) || (snakeBodyY[0] <= 0 && dir_y == -1)||(flagCollision))
        {
            LCD_text(loser, 1);
            flaglose=1;
        }

    }
    //vuelve a dibujar en la LCD
    refresh_buffer();
}

int main(void) {

    volatile int delay_count; // volatile so C compiler doesn't remove the loop

    //Inicializacion de interrupciones
    set_A9_IRQ_stack();										//initialize  stack pointer
	config_GIC();											//configure the general interrupt controller
	config_KEYS();											//configure pushbutton KEYS to generate interrupts
	enable_A9_interrupts();									// enable interrupts in the A9 processor
    init_spim0();
    init_lcd();

    //Limpia la pantalla
    clear_screen();

    while (1) {
        
        //Checa los valores de los switches para empezar o no con el juego
        START();

        //Dibuja el cuadro
        LCD_rect(randx, randy, length, 1);

        for (delay_count = 200000; delay_count != 0; --delay_count); // delay loop
    }
}

