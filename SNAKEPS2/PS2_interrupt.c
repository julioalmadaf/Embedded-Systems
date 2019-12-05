#include "address_map_arm.h"
#include "lcd_driver.h"
#include "lcd_graphic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

volatile int * SW = (int *)SW_BASE;				//SWITCH REGISTER
volatile int *LEDR_PTR = (int *)0xFF200000;

int julio;
int score;
int flagClear=0;

int x, y, length;
volatile int dir_x=1, dir_y=1;
volatile int flagx=1, flagy=0;
int flaglose=0;
int flagCollision=0;
int randx = 0;
int randy = 0;
int snakeLength;
int snakeBodyX[255];
int snakeBodyY[255];
volatile int toogleX=1;
volatile int toogleY=0;

/* create a message to be displayed on the VGA display */
//Menu principal
char text_top_lcd[17]    = "  ANY SWITCH TO \0";
char text_bottom_lcd[17] = "      START     \0";
//Pantalla de perdedor
char loser[17] = "      LOSER     \0";

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_MPcore_private_timer(void);
void config_PS2(void);
void enable_A9_interrupts(void);

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
        
		if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x74))
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
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x72))
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
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x75))
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
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x6B))
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
	return;
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
        //CHECAR ESTO*************************************************************
        flaglose=0;
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
        toogleX=0;
        toogleY=1;
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

        //Borra el ultimo cuadro
        LCD_rect(snakeBodyX[snakeLength], snakeBodyY[snakeLength], length, 0);

        //Imprime el largo de la serpiente
        int i,j;
        int SLtemp = snakeLength;
        SLtemp-=1;
        for(i=snakeLength, j=SLtemp; i>0 ; i--, j--)
        {
            snakeBodyX[i]=snakeBodyX[j];
            snakeBodyY[i]=snakeBodyY[j];
        }

        //Mueve a la serpiente
        if (flaglose==0)
        {
            //Imprime el cuadro en una nueva posicion
            if(flagx) snakeBodyX[0] += dir_x;
            if(flagy) snakeBodyY[0] += dir_y;
        }
        
        //Dibuja el cuadro nuevo
        int k;
        for(k=0; k<=snakeLength-1;k++)
        {
            LCD_rect(snakeBodyX[k], snakeBodyY[k], length, 1);
        }

        //Se come a si misma
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

    while (1) {

        START();

        //Dibuja el cuadro
        LCD_rect(randx, randy, length, 1);

        for (delay_count = 200000; delay_count != 0; --delay_count); // delay loop

        while (!timeout); // wait to synchronize with timer

        /* display PS/2 data (from interrupt service routine) on HEX displays */
        HEX_PS2(byte1, byte2, byte3);
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