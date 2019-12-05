#include <stdio.h>

void main()
{
	int LIST[8]={7,4,5,3,6,1,9,254};
	int *LEDR = (int *) 0xFF200000;
	int large = 0, i; //jeje
	
	
	for(i = 0; i < 8; i++)
	{
		if(large<LIST[i]) large=LIST[i];
	}
	
	*(LEDR) = large;
	
	printf("\n Largest number is: %d",large);
	
}