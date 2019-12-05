#include <stdio.h>
#include <stdbool.h>
#include "address_map_arm.h"

#define STANDARD_X 320
#define STANDARD_Y 240

volatile int pixel_buffer_start;					//global variable

volatile int *  BUFFER = (int *) PIXEL_BUF_CTRL_BASE;					//BUFFER REGISTER
volatile int *  BBUFFER = (int *) (PIXEL_BUF_CTRL_BASE + 0x4);			//BACKBUFFER REGISTER
volatile int * RESOLUTION = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);			//RESOLUTION REGISTER
volatile int * STATUS = (int *)(PIXEL_BUF_CTRL_BASE + 0xC);				//RESOLUTION REGISTER
	

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
            *(short int*)pixel_ptr = 0; // set pixel color
        }
}

int valueY =0;
int flag =0;

void moveLine()
{
	int j = valueY;
	if(flag==0)
	{
		valueY++;
		if(valueY == 239) flag = 1;
	}
	else
	{
		valueY--;
		if(valueY == 0) flag = 0;
	}
	draw_line(0,319,j,j,0);
	draw_line(0,319,valueY,valueY,0x07E0);
}

void VSYNC()
{
	moveLine();
	*(BUFFER) = 0x1;
	int bit;
	do{
		bit = *(STATUS);
	}while( (bit&1)==1);
}

int main(void)
{
	
	*(BUFFER+1) = 0XC8000000;													//BACKBUFFER USES THE SAME ADDRESS AS TEH BUFFER
	
	//Read the location of the pixel buffer from the pixel buffer controller
	pixel_buffer_start= * BUFFER;
	
	CLEAR();
	
	while(1)
	{		
		VSYNC();
	}
		
}
