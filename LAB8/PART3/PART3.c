#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "address_map_arm.h"

#define STANDARD_X 320
#define STANDARD_Y 240

volatile int pixel_buffer_start;					//global variable

volatile int *  BUFFER = (int *) PIXEL_BUF_CTRL_BASE;					//BUFFER REGISTER
volatile int *  BBUFFER = (int *) (PIXEL_BUF_CTRL_BASE + 0x4);			//BACKBUFFER REGISTER
volatile int * RESOLUTION = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);			//RESOLUTION REGISTER
volatile int * STATUS = (int *)(PIXEL_BUF_CTRL_BASE + 0xC);				//RESOLUTION REGISTER
	
void VSYNC()
{
	*(BUFFER) = 0x1;														//SWAP BETWEEN BUFFERS HAPPEN
	while(*(STATUS)==0x1);
	*(BUFFER) = 0x0;
}

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
            *(short *)pixel_ptr = 0; // set pixel color
        }
}

int valueY =0;
int flag =0;

void moveLine()
{
	draw_line(0,319,valueY,valueY,0x07E0);										//THIS LINE IS BLUE
	if(flag==0)
	{
		valueY++;
		if(valueY == 239) flag =1;
	}
	else
	{
		valueY--;
		if(valueY == 0) flag=0;
	}
}

void VGA_rect(int x1, int y1, int width, short int color)
{
    int x2 = x1 + width;
    int y2 = y1 + width;
    int i;
    for (; y1 <= y2; y1++)
	{
		draw_line(x1, x2, y1,  y1, color);
	}
}

int main(void)
{
	pixel_buffer_start= *BUFFER;
	//*(BUFFER+1) = 0XC8000000;													//BACKBUFFER USES THE SAME ADDRESS AS TEH BUFFER
	CLEAR();
	int length=5;
	int x0,x1,x2,x3,x4,x5,x6,x7,y0,y1,y2,y3,y4,y5,y6,y7;
		
	
	VGA_rect( x0 = rand() % 319, y0 = rand() %239, length, 0x07E0);
	VGA_rect( x1 = rand() % 319, y1 = rand() %239, length, 0x07E0);
	VGA_rect( x2 = rand() % 319, y2 = rand() %239, length, 0x07E0);
	VGA_rect( x3 = rand() % 319, y3 = rand() %239, length, 0x07E0);
	VGA_rect( x4 = rand() % 319, y4 = rand() %239, length, 0x07E0);
	VGA_rect( x5 = rand() % 319, y5 = rand() %239, length, 0x07E0);
	VGA_rect( x6 = rand() % 319, y6 = rand() %239, length, 0x07E0);
	VGA_rect( x7 = rand() % 319, y7 = rand() %239, length, 0x07E0);
	
	draw_line(x0,x1,y0,y1,0x07E0);
	draw_line(x1,x2,y1,y2,0x07E0);
	draw_line(x2,x3,y2,y3,0x07E0);
	draw_line(x3,x4,y3,y4,0x07E0);
	draw_line(x4,x5,y4,y5,0x07E0);
	draw_line(x5,x6,y5,y6,0x07E0);
	draw_line(x6,x7,y6,y7,0x07E0);
	draw_line(x7,x0,y7,y0,0x07E0);
	
	
	//VSYNC();
	//pixel_buffer_start=*BUFFER;
	//CLEAR();
	//*(BUFFER+1)=0xC0000000;
	//pixel_buffer_start=*(BUFFER+1);
	while(1)
	{
		//CLEAR();															// clear the screen
		//VGA_rect( rand() % 319, rand() %239, length, 0x07E0);
		//VSYNC();
		//pixel_buffer_start=*(BUFFER+1);
	}
}
