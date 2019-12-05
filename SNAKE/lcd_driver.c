#include "lcd_driver.h"
#include "address_map_arm.h"

void spim_write(int data)
{
    volatile int * spim0_sr = (int *) SPIM0_SR;
    volatile int * spim0_dr = (int *) SPIM0_DR;
    while (((*spim0_sr) & 0x4) != 0x4)
        ; // check status reg for empty
    (*spim0_dr) = data;
    while (((*spim0_sr) & 0x4) != 0x4)
        ; // check fifo is empty
    while (((*spim0_sr) & 0x1) != 0x0)
        ; // check spim has completed the transfer
}

void init_spim0(void)
{
    volatile int * rstmgr_premodrst = (int *) HPS_RSTMGR_PREMODRST;
    volatile int * spim0            = (int *) SPIM0_BASE;

    // Take SPIM0 out of reset
    *rstmgr_premodrst = *rstmgr_premodrst & (~0x00040000);

    // Turn SPIM0 OFF
    *(spim0 + 2) = 0x00000000;

    // Put SPIM0 in Tx Only Mode
    *(spim0 + 0) = *(spim0 + 0) & ~0x00000300;
    *(spim0 + 0) = *(spim0 + 0) | 0x00000100;

    // Set SPIM0 BAUD RATE
    *(spim0 + 5) = 0x00000040;

    // Set SPIM0 Slave Enable Register
    *(spim0 + 4) = 0x00000001;

    // Turn off interrupts
    *(spim0 + 11) = 0x00000000;

    // Turn SPIM0 ON
    *(spim0 + 2) = 0x00000001;
}

void init_lcd(void)
{
    volatile int * gpio1 = (int *) HPS_GPIO1_BASE;
    // Set GPIO1's direction register for the outputs to the LCD
    *(gpio1 + 1) = *(gpio1 + 1) | 0x00009100;
    // Turn on the LCD Backlight and take it out of reset
    *(gpio1) = 0x00008100;

    // Initialize LCD's registers
    spim_write(0x000000C8);
    spim_write(0x0000002F);
    spim_write(0x00000040);
    spim_write(0x000000B0);
    spim_write(0x00000000);
    spim_write(0x00000010);
    spim_write(0x000000AF);
}

/* 
 * Sets the mode of GPIO1.
 *
 * mode: 1 for command mode, 0 for data mode 
 */
void set_mode(int mode)
{
    volatile int * gpio1 = (int *) HPS_GPIO1_BASE;

    if (mode) // Enter command mode
        *(gpio1) = (*gpio1) & (~0x00001000);
    else // Enter data mode
        *(gpio1) = (*gpio1) | (0x00001000);
}
