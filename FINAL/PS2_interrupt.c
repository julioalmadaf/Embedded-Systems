#include "address_map_arm.h"
#include "lcd_driver.h"
#include "lcd_graphic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_MPcore_private_timer(void);
void config_PS2(void);
void enable_A9_interrupts(void);

/* these globals are written by interrupt service routines; we declare them as
 * volatile to avoid the compiler caching their values, even in registers */
extern volatile char byte1, byte2, byte3; /* modified by PS/2 interrupt service routine */
extern volatile int timeout; // used to synchronize with the timer
char key=0;
char flagCaps=0;
char flagEnter=0;enter=0;
char *text= "\0",*text1= "\0", *text2= "\0",*text3= "\0", *text4= "\0", *text5= "\0", *text6= "\0", *text7= "\0";
int first=1;

/* function prototypes */
void HEX_PS2(char, char, char);


void PS2_ISR( void )
{
  	volatile int * PS2_ptr = (int *) 0xFF200100;		// PS/2 port address
	int PS2_data, RAVAIL;

	PS2_data = *(PS2_ptr);									// read the Data register in the PS/2 port
	RAVAIL = (PS2_data & 0xFFFF0000) >> 16;			// extract the RAVAIL field
	if (RAVAIL > 0)
	{
		/* always save the last three bytes received */
		//byte1 = byte2;
		byte2 = byte3;
		byte3 = PS2_data & 0xFF;
	}
    key=1;
	return;
}

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    strcat(result, "\0");
    return result;
}

void checKey0()
{

    if(byte3==0x66) text[strlen(text)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text=concat(text,"|");
        else if(byte3==0x16) text=concat(text,"1");
        else if(byte3==0x1E) text=concat(text,"2");
        else if(byte3==0x26) text=concat(text,"3");
        else if(byte3==0x25) text=concat(text,"4");
        else if(byte3==0x2E) text=concat(text,"5");
        else if(byte3==0x36) text=concat(text,"6");
        else if(byte3==0x3D) text=concat(text,"7");
        else if(byte3==0x3E) text=concat(text,"8");
        else if(byte3==0x46) text=concat(text,"9");
        else if(byte3==0x45) text=concat(text,"0");
        else if(byte3==0x4E) text=concat(text,"'");
        else if(byte3==0x55) text=concat(text,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text=concat(text,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text=concat(text,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text=concat(text,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text=concat(text,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text=concat(text,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text=concat(text,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text=concat(text,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text=concat(text,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text=concat(text,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text=concat(text,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text=concat(text,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text=concat(text,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text=concat(text,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text=concat(text,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text=concat(text,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text=concat(text,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text=concat(text,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text=concat(text,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text=concat(text,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text=concat(text,"P");
        else if(byte3==0x54) text=concat(text,"´");
        else if(byte3==0x5B) text=concat(text,"+");
        else if(byte3==0x58) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text=concat(text,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text=concat(text,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text=concat(text,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text=concat(text,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text=concat(text,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text=concat(text,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text=concat(text,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text=concat(text,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text=concat(text,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text=concat(text,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text=concat(text,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text=concat(text,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text=concat(text,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text=concat(text,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text=concat(text,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text=concat(text,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text=concat(text,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text=concat(text,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text=concat(text,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text=concat(text,"z");
        else if((byte3==0x22)&& (flagCaps==0))text=concat(text,"x");
        else if((byte3==0x21)&& (flagCaps==0))text=concat(text,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text=concat(text,"v");
        else if((byte3==0x32)&& (flagCaps==0))text=concat(text,"b");
        else if((byte3==0x31)&& (flagCaps==0))text=concat(text,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text=concat(text,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text=concat(text,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text=concat(text,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text=concat(text,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text=concat(text,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text=concat(text,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text=concat(text,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text=concat(text,"M");
        else if(byte3==0x41) text=concat(text,",");
        else if(byte3==0x49) text=concat(text,".");
        else if(byte3==0x4A) text=concat(text,"-");
        else if(byte3==0x29) text=concat(text," "); 
    }
}

void checKey1()
{
    if(byte3==0x66) text1[strlen(text1)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text1=concat(text1,"|");
        else if(byte3==0x16) text1=concat(text1,"1");
        else if(byte3==0x1E) text1=concat(text1,"2");
        else if(byte3==0x26) text1=concat(text1,"3");
        else if(byte3==0x25) text1=concat(text1,"4");
        else if(byte3==0x2E) text1=concat(text1,"5");
        else if(byte3==0x36) text1=concat(text1,"6");
        else if(byte3==0x3D) text1=concat(text1,"7");
        else if(byte3==0x3E) text1=concat(text1,"8");
        else if(byte3==0x46) text1=concat(text1,"9");
        else if(byte3==0x45) text1=concat(text1,"0");
        else if(byte3==0x4E) text1=concat(text1,"'");
        else if(byte3==0x55) text1=concat(text1,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text1=concat(text1,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text1=concat(text1,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text1=concat(text1,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text1=concat(text1,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text1=concat(text1,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text1=concat(text1,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text1=concat(text1,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text1=concat(text1,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text1=concat(text1,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text1=concat(text1,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text1=concat(text1,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text1=concat(text1,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text1=concat(text1,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text1=concat(text1,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text1=concat(text1,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text1=concat(text1,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text1=concat(text1,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text1=concat(text1,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text1=concat(text1,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text1=concat(text1,"P");
        else if(byte3==0x54) text1=concat(text1,"´");
        else if(byte3==0x5B) text1=concat(text1,"+");
        else if(byte3==0x58) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text1=concat(text1,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text1=concat(text1,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text1=concat(text1,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text1=concat(text1,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text1=concat(text1,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text1=concat(text1,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text1=concat(text1,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text1=concat(text1,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text1=concat(text1,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text1=concat(text1,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text1=concat(text1,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text1=concat(text1,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text1=concat(text1,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text1=concat(text1,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text1=concat(text1,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text1=concat(text1,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text1=concat(text1,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text1=concat(text1,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text1=concat(text1,"Ñ");
        else if((byte3==0x5A)&&(byte2==0xF0)&&(byte3==0x5A))flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text1=concat(text1,"z");
        else if((byte3==0x22)&& (flagCaps==0))text1=concat(text1,"x");
        else if((byte3==0x21)&& (flagCaps==0))text1=concat(text1,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text1=concat(text1,"v");
        else if((byte3==0x32)&& (flagCaps==0))text1=concat(text1,"b");
        else if((byte3==0x31)&& (flagCaps==0))text1=concat(text1,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text1=concat(text1,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text1=concat(text1,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text1=concat(text1,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text1=concat(text1,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text1=concat(text1,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text1=concat(text1,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text1=concat(text1,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text1=concat(text1,"M");
        else if(byte3==0x41) text1=concat(text1,",");
        else if(byte3==0x49) text1=concat(text1,".");
        else if(byte3==0x4A) text1=concat(text1,"-");
        else if(byte3==0x29) text1=concat(text1," "); 
    }
}

void checKey2()
{
    if(byte3==0x66) text2[strlen(text2)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text2=concat(text2,"|");
        else if(byte3==0x16) text2=concat(text2,"1");
        else if(byte3==0x1E) text2=concat(text2,"2");
        else if(byte3==0x26) text2=concat(text2,"3");
        else if(byte3==0x25) text2=concat(text2,"4");
        else if(byte3==0x2E) text2=concat(text2,"5");
        else if(byte3==0x36) text2=concat(text2,"6");
        else if(byte3==0x3D) text2=concat(text2,"7");
        else if(byte3==0x3E) text2=concat(text2,"8");
        else if(byte3==0x46) text2=concat(text2,"9");
        else if(byte3==0x45) text2=concat(text2,"0");
        else if(byte3==0x4E) text2=concat(text2,"'");
        else if(byte3==0x55) text2=concat(text2,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text2=concat(text2,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text2=concat(text2,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text2=concat(text2,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text2=concat(text2,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text2=concat(text2,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text2=concat(text2,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text2=concat(text2,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text2=concat(text2,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text2=concat(text2,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text2=concat(text2,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text2=concat(text2,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text2=concat(text2,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text2=concat(text2,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text2=concat(text2,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text2=concat(text2,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text2=concat(text2,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text2=concat(text2,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text2=concat(text2,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text2=concat(text2,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text2=concat(text2,"P");
        else if(byte3==0x54) text2=concat(text2,"´");
        else if(byte3==0x5B) text2=concat(text2,"+");
        else if(byte3==0x58) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text2=concat(text2,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text2=concat(text2,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text2=concat(text2,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text2=concat(text2,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text2=concat(text2,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text2=concat(text2,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text2=concat(text2,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text2=concat(text2,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text2=concat(text2,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text2=concat(text2,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text2=concat(text2,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text2=concat(text2,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text2=concat(text2,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text2=concat(text2,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text2=concat(text2,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text2=concat(text2,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text2=concat(text2,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text2=concat(text2,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text2=concat(text2,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text2=concat(text2,"z");
        else if((byte3==0x22)&& (flagCaps==0))text2=concat(text2,"x");
        else if((byte3==0x21)&& (flagCaps==0))text2=concat(text2,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text2=concat(text2,"v");
        else if((byte3==0x32)&& (flagCaps==0))text2=concat(text2,"b");
        else if((byte3==0x31)&& (flagCaps==0))text2=concat(text2,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text2=concat(text2,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text2=concat(text2,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text2=concat(text2,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text2=concat(text2,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text2=concat(text2,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text2=concat(text2,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text2=concat(text2,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text2=concat(text2,"M");
        else if(byte3==0x41) text2=concat(text2,",");
        else if(byte3==0x49) text2=concat(text2,".");
        else if(byte3==0x4A) text2=concat(text2,"-");
        else if(byte3==0x29) text2=concat(text2," ");
    } 
}

void checKey3()
{
    if(byte3==0x66) text3[strlen(text3)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text3=concat(text3,"|");
        else if(byte3==0x16) text3=concat(text3,"1");
        else if(byte3==0x1E) text3=concat(text3,"2");
        else if(byte3==0x26) text3=concat(text3,"3");
        else if(byte3==0x25) text3=concat(text3,"4");
        else if(byte3==0x2E) text3=concat(text3,"5");
        else if(byte3==0x36) text3=concat(text3,"6");
        else if(byte3==0x3D) text3=concat(text3,"7");
        else if(byte3==0x3E) text3=concat(text3,"8");
        else if(byte3==0x46) text3=concat(text3,"9");
        else if(byte3==0x45) text3=concat(text3,"0");
        else if(byte3==0x4E) text3=concat(text3,"'");
        else if(byte3==0x55) text3=concat(text3,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text3=concat(text3,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text3=concat(text3,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text3=concat(text3,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text3=concat(text3,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text3=concat(text3,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text3=concat(text3,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text3=concat(text3,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text3=concat(text3,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text3=concat(text3,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text3=concat(text3,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text3=concat(text3,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text3=concat(text3,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text3=concat(text3,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text3=concat(text3,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text3=concat(text3,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text3=concat(text3,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text3=concat(text3,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text3=concat(text3,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text3=concat(text3,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text3=concat(text3,"P");
        else if(byte3==0x54) text3=concat(text3,"´");
        else if(byte3==0x5B) text3=concat(text3,"+");
        else if((byte3==0x58)&&(byte2==0xF0)&&(byte3==0x58)) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text3=concat(text3,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text3=concat(text3,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text3=concat(text3,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text3=concat(text3,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text3=concat(text3,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text3=concat(text3,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text3=concat(text3,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text3=concat(text3,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text3=concat(text3,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text3=concat(text3,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text3=concat(text3,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text3=concat(text3,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text3=concat(text3,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text3=concat(text3,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text3=concat(text3,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text3=concat(text3,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text3=concat(text3,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text3=concat(text3,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text3=concat(text3,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text3=concat(text3,"z");
        else if((byte3==0x22)&& (flagCaps==0))text3=concat(text3,"x");
        else if((byte3==0x21)&& (flagCaps==0))text3=concat(text3,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text3=concat(text3,"v");
        else if((byte3==0x32)&& (flagCaps==0))text3=concat(text3,"b");
        else if((byte3==0x31)&& (flagCaps==0))text3=concat(text3,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text3=concat(text3,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text3=concat(text3,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text3=concat(text3,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text3=concat(text3,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text3=concat(text3,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text3=concat(text3,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text3=concat(text3,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text3=concat(text3,"M");
        else if(byte3==0x41) text3=concat(text3,",");
        else if(byte3==0x49) text3=concat(text3,".");
        else if(byte3==0x4A) text3=concat(text3,"-");
        else if(byte3==0x29) text3=concat(text3," "); 
    }
}

void checKey4()
{
    if(byte3==0x66) text4[strlen(text4)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text4=concat(text4,"|");
        else if(byte3==0x16) text4=concat(text4,"1");
        else if(byte3==0x1E) text4=concat(text4,"2");
        else if(byte3==0x26) text4=concat(text4,"3");
        else if(byte3==0x25) text4=concat(text4,"4");
        else if(byte3==0x2E) text4=concat(text4,"5");
        else if(byte3==0x36) text4=concat(text4,"6");
        else if(byte3==0x3D) text4=concat(text4,"7");
        else if(byte3==0x3E) text4=concat(text4,"8");
        else if(byte3==0x46) text4=concat(text4,"9");
        else if(byte3==0x45) text4=concat(text4,"0");
        else if(byte3==0x4E) text4=concat(text4,"'");
        else if(byte3==0x55) text4=concat(text4,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text4=concat(text4,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text4=concat(text4,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text4=concat(text4,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text4=concat(text4,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text4=concat(text4,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text4=concat(text4,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text4=concat(text4,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text4=concat(text4,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text4=concat(text4,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text4=concat(text4,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text4=concat(text4,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text4=concat(text4,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text4=concat(text4,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text4=concat(text4,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text4=concat(text4,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text4=concat(text4,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text4=concat(text4,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text4=concat(text4,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text4=concat(text4,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text4=concat(text4,"P");
        else if(byte3==0x54) text4=concat(text4,"´");
        else if(byte3==0x5B) text4=concat(text4,"+");
        else if((byte3==0x58)&&(byte2==0xF0)&&(byte3==0x58)) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text4=concat(text4,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text4=concat(text4,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text4=concat(text4,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text4=concat(text4,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text4=concat(text4,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text4=concat(text4,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text4=concat(text4,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text4=concat(text4,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text4=concat(text4,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text4=concat(text4,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text4=concat(text4,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text4=concat(text4,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text4=concat(text4,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text4=concat(text4,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text4=concat(text4,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text4=concat(text4,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text4=concat(text4,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text4=concat(text4,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text4=concat(text4,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text4=concat(text4,"z");
        else if((byte3==0x22)&& (flagCaps==0))text4=concat(text4,"x");
        else if((byte3==0x21)&& (flagCaps==0))text4=concat(text4,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text4=concat(text4,"v");
        else if((byte3==0x32)&& (flagCaps==0))text4=concat(text4,"b");
        else if((byte3==0x31)&& (flagCaps==0))text4=concat(text4,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text4=concat(text4,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text4=concat(text4,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text4=concat(text4,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text4=concat(text4,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text4=concat(text4,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text4=concat(text4,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text4=concat(text4,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text4=concat(text4,"M");
        else if(byte3==0x41) text4=concat(text4,",");
        else if(byte3==0x49) text4=concat(text4,".");
        else if(byte3==0x4A) text4=concat(text4,"-");
        else if(byte3==0x29) text4=concat(text4," "); 
    }
}

void checKey5()
{
    if(byte3==0x66) text5[strlen(text5)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text5=concat(text5,"|");
        else if(byte3==0x16) text5=concat(text5,"1");
        else if(byte3==0x1E) text5=concat(text5,"2");
        else if(byte3==0x26) text5=concat(text5,"3");
        else if(byte3==0x25) text5=concat(text5,"4");
        else if(byte3==0x2E) text5=concat(text5,"5");
        else if(byte3==0x36) text5=concat(text5,"6");
        else if(byte3==0x3D) text5=concat(text5,"7");
        else if(byte3==0x3E) text5=concat(text5,"8");
        else if(byte3==0x46) text5=concat(text5,"9");
        else if(byte3==0x45) text5=concat(text5,"0");
        else if(byte3==0x4E) text5=concat(text5,"'");
        else if(byte3==0x55) text5=concat(text5,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text5=concat(text5,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text5=concat(text5,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text5=concat(text5,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text5=concat(text5,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text5=concat(text5,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text5=concat(text5,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text5=concat(text5,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text5=concat(text5,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text5=concat(text5,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text5=concat(text5,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text5=concat(text5,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text5=concat(text5,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text5=concat(text5,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text5=concat(text5,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text5=concat(text5,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text5=concat(text5,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text5=concat(text5,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text5=concat(text5,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text5=concat(text5,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text5=concat(text5,"P");
        else if(byte3==0x54) text5=concat(text5,"´");
        else if(byte3==0x5B) text5=concat(text5,"+");
        else if((byte3==0x58)&&(byte2==0xF0)&&(byte3==0x58)) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text5=concat(text5,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text5=concat(text5,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text5=concat(text5,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text5=concat(text5,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text5=concat(text5,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text5=concat(text5,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text5=concat(text5,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text5=concat(text5,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text5=concat(text5,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text5=concat(text5,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text5=concat(text5,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text5=concat(text5,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text5=concat(text5,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text5=concat(text5,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text5=concat(text5,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text5=concat(text5,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text5=concat(text5,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text5=concat(text5,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text5=concat(text5,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text5=concat(text5,"z");
        else if((byte3==0x22)&& (flagCaps==0))text5=concat(text5,"x");
        else if((byte3==0x21)&& (flagCaps==0))text5=concat(text5,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text5=concat(text5,"v");
        else if((byte3==0x32)&& (flagCaps==0))text5=concat(text5,"b");
        else if((byte3==0x31)&& (flagCaps==0))text5=concat(text5,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text5=concat(text5,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text5=concat(text5,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text5=concat(text5,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text5=concat(text5,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text5=concat(text5,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text5=concat(text5,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text5=concat(text5,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text5=concat(text5,"M");
        else if(byte3==0x41) text5=concat(text5,",");
        else if(byte3==0x49) text5=concat(text5,".");
        else if(byte3==0x4A) text5=concat(text5,"-");
        else if(byte3==0x29) text5=concat(text5," "); 
    }
}

void checKey6()
{
    if(byte3==0x66) text6[strlen(text6)-1]=0; //Backspace
    else
        {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text6=concat(text6,"|");
        else if(byte3==0x16) text6=concat(text6,"1");
        else if(byte3==0x1E) text6=concat(text6,"2");
        else if(byte3==0x26) text6=concat(text6,"3");
        else if(byte3==0x25) text6=concat(text6,"4");
        else if(byte3==0x2E) text6=concat(text6,"5");
        else if(byte3==0x36) text6=concat(text6,"6");
        else if(byte3==0x3D) text6=concat(text6,"7");
        else if(byte3==0x3E) text6=concat(text6,"8");
        else if(byte3==0x46) text6=concat(text6,"9");
        else if(byte3==0x45) text6=concat(text6,"0");
        else if(byte3==0x4E) text6=concat(text6,"'");
        else if(byte3==0x55) text6=concat(text6,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text6=concat(text6,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text6=concat(text6,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text6=concat(text6,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text6=concat(text6,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text6=concat(text6,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text6=concat(text6,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text6=concat(text6,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text6=concat(text6,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text6=concat(text6,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text6=concat(text6,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text6=concat(text6,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text6=concat(text6,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text6=concat(text6,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text6=concat(text6,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text6=concat(text6,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text6=concat(text6,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text6=concat(text6,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text6=concat(text6,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text6=concat(text6,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text6=concat(text6,"P");
        else if(byte3==0x54) text6=concat(text6,"´");
        else if(byte3==0x5B) text6=concat(text6,"+");
        else if((byte3==0x58)&&(byte2==0xF0)&&(byte3==0x58)) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text6=concat(text6,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text6=concat(text6,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text6=concat(text6,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text6=concat(text6,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text6=concat(text6,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text6=concat(text6,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text6=concat(text6,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text6=concat(text6,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text6=concat(text6,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text6=concat(text6,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text6=concat(text6,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text6=concat(text6,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text6=concat(text6,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text6=concat(text6,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text6=concat(text6,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text6=concat(text6,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text6=concat(text6,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text6=concat(text6,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text6=concat(text6,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text6=concat(text6,"z");
        else if((byte3==0x22)&& (flagCaps==0))text6=concat(text6,"x");
        else if((byte3==0x21)&& (flagCaps==0))text6=concat(text6,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text6=concat(text6,"v");
        else if((byte3==0x32)&& (flagCaps==0))text6=concat(text6,"b");
        else if((byte3==0x31)&& (flagCaps==0))text6=concat(text6,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text6=concat(text6,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text6=concat(text6,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text6=concat(text6,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text6=concat(text6,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text6=concat(text6,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text6=concat(text6,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text6=concat(text6,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text6=concat(text6,"M");
        else if(byte3==0x41) text6=concat(text6,",");
        else if(byte3==0x49) text6=concat(text6,".");
        else if(byte3==0x4A) text6=concat(text6,"-");
        else if(byte3==0x29) text6=concat(text6," ");
    }
}

void checKey7()
{
    if(byte3==0x66) text7[strlen(text7)-1]=0; //Backspace
    else
    {
        while(byte2!=0xF0){};
        if(byte3==0x0E) text7=concat(text7,"|");
        else if(byte3==0x16) text7=concat(text7,"1");
        else if(byte3==0x1E) text7=concat(text7,"2");
        else if(byte3==0x26) text7=concat(text7,"3");
        else if(byte3==0x25) text7=concat(text7,"4");
        else if(byte3==0x2E) text7=concat(text7,"5");
        else if(byte3==0x36) text7=concat(text7,"6");
        else if(byte3==0x3D) text7=concat(text7,"7");
        else if(byte3==0x3E) text7=concat(text7,"8");
        else if(byte3==0x46) text7=concat(text7,"9");
        else if(byte3==0x45) text7=concat(text7,"0");
        else if(byte3==0x4E) text7=concat(text7,"'");
        else if(byte3==0x55) text7=concat(text7,"¿");
        else if((byte3==0x15)&& (flagCaps==0)) text7=concat(text7,"q");
        else if((byte3==0x1D)&& (flagCaps==0)) text7=concat(text7,"w");
        else if((byte3==0x24)&& (flagCaps==0)) text7=concat(text7,"e");
        else if((byte3==0x2D)&& (flagCaps==0)) text7=concat(text7,"r");
        else if((byte3==0x2C)&& (flagCaps==0)) text7=concat(text7,"t");
        else if((byte3==0x35)&& (flagCaps==0)) text7=concat(text7,"y");
        else if((byte3==0x3C)&& (flagCaps==0)) text7=concat(text7,"u");
        else if((byte3==0x43)&& (flagCaps==0)) text7=concat(text7,"i");
        else if((byte3==0x44)&& (flagCaps==0)) text7=concat(text7,"o");
        else if((byte3==0x4D)&& (flagCaps==0)) text7=concat(text7,"p");
        else if((byte3==0x15)&& (flagCaps==1)) text7=concat(text7,"Q");
        else if((byte3==0x1D)&& (flagCaps==1)) text7=concat(text7,"W");
        else if((byte3==0x24)&& (flagCaps==1)) text7=concat(text7,"E");
        else if((byte3==0x2D)&& (flagCaps==1)) text7=concat(text7,"R");
        else if((byte3==0x2C)&& (flagCaps==1)) text7=concat(text7,"T");
        else if((byte3==0x35)&& (flagCaps==1)) text7=concat(text7,"Y");
        else if((byte3==0x3C)&& (flagCaps==1)) text7=concat(text7,"U");
        else if((byte3==0x43)&& (flagCaps==1)) text7=concat(text7,"I");
        else if((byte3==0x44)&& (flagCaps==1)) text7=concat(text7,"O");
        else if((byte3==0x4D)&& (flagCaps==1)) text7=concat(text7,"P");
        else if(byte3==0x54) text7=concat(text7,"´");
        else if(byte3==0x5B) text7=concat(text7,"+");
        else if((byte3==0x58)&&(byte2==0xF0)&&(byte3==0x58)) //MAYUS
        { 
            if(flagCaps==0)flagCaps=1;
            else if(flagCaps==1)flagCaps=0;
        }
        else if((byte3==0x1C)&& (flagCaps==0)) text7=concat(text7,"a");
        else if((byte3==0x1B)&& (flagCaps==0)) text7=concat(text7,"s");
        else if((byte3==0x23)&& (flagCaps==0)) text7=concat(text7,"d");
        else if((byte3==0x2B)&& (flagCaps==0)) text7=concat(text7,"f");
        else if((byte3==0x34)&& (flagCaps==0)) text7=concat(text7,"g");
        else if((byte3==0x33)&& (flagCaps==0)) text7=concat(text7,"h");
        else if((byte3==0x3B)&& (flagCaps==0)) text7=concat(text7,"j");
        else if((byte3==0x42)&& (flagCaps==0)) text7=concat(text7,"k");
        else if((byte3==0x4B)&& (flagCaps==0)) text7=concat(text7,"l");
        else if((byte3==0x1C)&& (flagCaps==1)) text7=concat(text7,"A");
        else if((byte3==0x1B)&& (flagCaps==1)) text7=concat(text7,"S");
        else if((byte3==0x23)&& (flagCaps==1)) text7=concat(text7,"D");
        else if((byte3==0x2B)&& (flagCaps==1)) text7=concat(text7,"F");
        else if((byte3==0x34)&& (flagCaps==1)) text7=concat(text7,"G");
        else if((byte3==0x33)&& (flagCaps==1)) text7=concat(text7,"H");
        else if((byte3==0x3B)&& (flagCaps==1)) text7=concat(text7,"J");
        else if((byte3==0x42)&& (flagCaps==1)) text7=concat(text7,"K");
        else if((byte3==0x4B)&& (flagCaps==1)) text7=concat(text7,"L");
        else if((byte3==0x4C)&& (flagCaps==1)) text7=concat(text7,"Ñ");
        else if(byte3==0x5A) flagEnter++; //ENTER 16 columnas 8 filas
        else if((byte3==0x1A)&& (flagCaps==0))text7=concat(text7,"z");
        else if((byte3==0x22)&& (flagCaps==0))text7=concat(text7,"x");
        else if((byte3==0x21)&& (flagCaps==0))text7=concat(text7,"c");
        else if((byte3==0x2A)&& (flagCaps==0))text7=concat(text7,"v");
        else if((byte3==0x32)&& (flagCaps==0))text7=concat(text7,"b");
        else if((byte3==0x31)&& (flagCaps==0))text7=concat(text7,"n");
        else if((byte3==0x3A)&& (flagCaps==0))text7=concat(text7,"m");
        else if((byte3==0x1A)&& (flagCaps==1)) text7=concat(text7,"Z");
        else if((byte3==0x22)&& (flagCaps==1)) text7=concat(text7,"X");
        else if((byte3==0x21)&& (flagCaps==1)) text7=concat(text7,"C");
        else if((byte3==0x2A)&& (flagCaps==1)) text7=concat(text7,"V");
        else if((byte3==0x32)&& (flagCaps==1)) text7=concat(text7,"B");
        else if((byte3==0x31)&& (flagCaps==1)) text7=concat(text7,"N");
        else if((byte3==0x3A)&& (flagCaps==1)) text7=concat(text7,"M");
        else if(byte3==0x41) text7=concat(text7,",");
        else if(byte3==0x49) text7=concat(text7,".");
        else if(byte3==0x4A) text7=concat(text7,"-");
        else if(byte3==0x29) text7=concat(text7," ");
    }
}

void printLCD()
{
    clear_screen();

    if(flagEnter!=0) 
    {
        enter++; 
        flagEnter=0;
        if(enter>7) enter=7;
    }
    
    if((enter==1)&& (byte3==0x66)&&(strlen(text1)==0)) enter=0;
    else if((enter==2)&& (byte3==0x66)&&(strlen(text2)==0)) enter=1;
    else if((enter==3)&& (byte3==0x66)&&(strlen(text3)==0)) enter=2;
    else if((enter==4)&& (byte3==0x66)&&(strlen(text4)==0)) enter=3;
    else if((enter==5)&& (byte3==0x66)&&(strlen(text5)==0)) enter=4;
    else if((enter==6)&& (byte3==0x66)&&(strlen(text6)==0)) enter=5;
    else if((enter==7)&& (byte3==0x66)&&(strlen(text7)==0)) enter=6;
    
    if((enter==0)&&(strlen(text)<17)) checKey0();
    else if((enter==1)&&(strlen(text1)<17)) checKey1();
	else if((enter==2)&&(strlen(text2)<17)) checKey2();
    else if((enter==3)&&(strlen(text3)<17)) checKey3();
	else if((enter==4)&&(strlen(text4)<17)) checKey4();
    else if((enter==5)&&(strlen(text5)<17)) checKey5();
	else if((enter==6)&&(strlen(text6)<17)) checKey6();
    else if((enter==7)&&(strlen(text7)<16)) checKey7();

    if((enter==0)&&(strlen(text)==16))enter++;
    else if((enter==1)&&(strlen(text1)==16))enter++;
    else if((enter==2)&&(strlen(text2)==16))enter++;
    else if((enter==3)&&(strlen(text3)==16))enter++;
    else if((enter==4)&&(strlen(text4)==16))enter++;
    else if((enter==5)&&(strlen(text5)==16))enter++;
    else if((enter==6)&&(strlen(text6)==16))enter++;
    else if((enter==7)&& (byte3==0x66)&&(strlen(text7)==16)) text7[strlen(text7)-1]=0; //Backspace

    LCD_text(text,0);
    LCD_text(text1,1);
    LCD_text(text2,2);
    LCD_text(text3,3);
    LCD_text(text4,4);
    LCD_text(text5,5);
    LCD_text(text6,6);
    LCD_text(text7,7);

    refresh_buffer();
}

int main(void) {
    /* initialize some variables */
    byte1        = 0;
    byte2        = 0;
    byte3        = 0; // used to hold PS/2 data
    timeout      = 0; // synchronize with the timer

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

        while (!timeout); // wait to synchronize with timer
        
        if(key==1)printLCD();
        key=0;

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