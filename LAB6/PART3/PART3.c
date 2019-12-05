#include <stdio.h>



int swnum(int num)
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

int div(int result)
{
	int result2=0,result3=0,result4=0;
	int ayuda= result%10;
	result-=ayuda;
	result=result/10;
	result2=swnum(ayuda);
	result3=swnum(result);
	result4=(result2|(result3<<8));
	return result4;
}

void main()
{
	int TEST[11] ={		0x0000e000, 0x3fabedef,0x00000001,
						0x00000002, 0x75a5a5a5, 0x01fff800,
						0x03ffC000, 0xFAAAAAAF, 0x77777777, 
						0x08888888, 0x00001111};
						
	int *seg1 = (int *) 0xFF200020;
	int *seg2 = (int *) 0xFF200030;
	
	int ones = 0, zeroes=0, alter=0, i; 
	int ch1, ch0,chalter;
	int primer=0, segundo=0;
	
	//ones
	for(i = 0; i < 10; i++)
	{
		ch1=0;
		primer=TEST[i];
		while(primer!=0x00000000)
		{
			segundo = primer<<1;
			primer = primer & segundo;
			ch1++;
		}
		if(ones<ch1) ones=ch1;
	}
	 
	for(i = 0; i < 10; i++)
	{
		ch0=0;
		primer=TEST[i];
		primer=~primer;
		while(primer!=0x00000000)
		{
			segundo = primer<<1;
			primer = primer & segundo;
			ch0++;
		}
		if(zeroes<ch0) zeroes=ch0;
	}
	
	for(i = 0; i < 10; i++)
	{
		chalter=0;
		primer=TEST[i];
		segundo=primer<<1;
		primer=primer^segundo;
		while(primer!=0x00000000)
		{
			segundo=primer<<1;
			primer = primer & segundo;
			chalter++;
		}
		if(alter < chalter) alter = chalter;
	}
	printf("%d,%d,%d", alter,zeroes,ones);
	*(seg1) = ((div(zeroes)<<16)|div(ones));
	*(seg2) = div(alter);
	
}