#ifndef _INC_LCD_DRIVER_H_
#define _INC_LCD_DRIVER_H_

void spim_write(int data);
void init_spim0(void);
void init_lcd(void);
void set_mode(int mode);

#endif
