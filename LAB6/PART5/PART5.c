#include <stdio.h>

int h1, h2, h3, h4, h5, h6;				//FOR EACH HEX 
int M,S,D;
int *seg1 = (int *) 0xFF200020;			//BASE ADDRESS HEX3-0
int *seg2 = (int *) 0xFF200030; 		//BASE ADDRESS HEX5-4
volatile int *buttons = (int *) 0xFF200050;		//BASE ADDDRESS BUTTONS
int counter=40000000;
int msg[13] ={0b1011110, 0b1111001, 0b0000110, 0b1000000, 0b1101101, 0b1011100, 0b0111001, 0b0000000, 0b0000000, 0b0000000, 0b0000000, 0b0000000, 0b0000000};

void acHex()
{
	h6=msg[1];
	h5=msg[2];
	h4=msg[3];
	h3=msg[4];
	h2=msg[5];
	h1=msg[6];
	M=(h5|(h6<<8));
	S=(h3|(h4<<8));
	D=(h1|(h2<<8));
}

void printHex()
{
	acHex();
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

void rotate(int arr[], int n, int k)
{
	while(k>0)
	{
		int temp=arr[0];
		int i;
		for(i=0;i<n;i++)
		{
			arr[i]=arr[i+1]; 							//array elements shifting
		}
		arr[n-1]=temp;
		k--;
	}
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
		buttoncheck();
		printHex();
		rotate(msg,13,1);
	}
		
}