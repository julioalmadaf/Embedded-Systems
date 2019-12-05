#include <stdio.h>

void main()
{
	int LIST[8]={7,4,5,3,6,1,8,2};
	int large = 0, i;
	
	for(i = 0; i < 7; i++)
	{
		if(large<LIST[i]) large=LIST[i];
	}
	printf("\n Largest number is: %d",large);
}