#include <stdio.h>

int h1, h2, h3, h4, h5, h6;				//FOR EACH HEX 
int d1, d2, s1, s2, m1, m2; 			//MM:SS:DD
int M,S,D;
int *seg1 = (int *) 0xFF200020;			//BASE ADDRESS HEX3-0
int *seg2 = (int *) 0xFF200030; 		//BASE ADDRESS HEX5-4
volatile int *buttons = (int *) 0xFF200050;		//BASE ADDDRESS BUTTONS
int counter=2000000;

int counterReloj=0;
int flag=0;

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

void buttoncheck()
{
	if(*(buttons) != 0)
	{
		while(*(buttons) != 0);
		while(*(buttons) == 0);
	}
	while(*(buttons) != 0);
}

void main()
{	
	volatile int *pritimer = (int *) 0xFFFEC600;		//BASE ADDRESS A9 PRIVATE TIMER
	*(pritimer) = counter;
	*(pritimer + 2) = 0b011;
	
	for(;;)
	{	
		while(*(pritimer+3) == 0);
		*(pritimer+3)=1;
		counterReloj++;
		if(counterReloj == 599999) counterReloj=0;
		buttoncheck();
		div(counterReloj);
		printHex();
	}
		
}