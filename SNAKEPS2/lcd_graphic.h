#ifndef _INC_LCD_GRAPHIC_H_
#define _INC_LCD_GRAPHIC_H_

#define FRAME_WIDTH 128
#define FRAME_HEIGHT 8
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void clear_screen(void);
void refresh_buffer(void);
void LCD_line(int x, int y, int length, int color, int vert);
void LCD_rect(int x1, int y1, int x2, int color);
void LCD_text(char * str, int page);

#endif
