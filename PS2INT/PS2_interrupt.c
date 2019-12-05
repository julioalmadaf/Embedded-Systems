#include "address_map_arm.h"

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_MPcore_private_timer(void);
void config_PS2(void);
void enable_A9_interrupts(void);

/* these globals are written by interrupt service routines; we declare them as
 * volatile to avoid the compiler caching their values, even in registers */
extern volatile char byte1, byte2,
    byte3; /* modified by PS/2 interrupt service routine */
extern volatile int timeout; // used to synchronize with the timer

/* function prototypes */
void HEX_PS2(char, char, char);

/********************************************************************************
 * This program performs the following:
 * 	   Displays the last three bytes of data received from the PS/2 port
 * 	   on the HEX displays on the board. The PS/2 port is handled using
 * 	   interrupts
********************************************************************************/
int main(void) {
    /* initialize some variables */
    byte1        = 0;
    byte2        = 0;
    byte3        = 0; // used to hold PS/2 data
    timeout      = 0; // synchronize with the timer

    set_A9_IRQ_stack();            // initialize the stack pointer for IRQ mode
    config_GIC();                  // configure the general interrupt controller
    config_MPcore_private_timer(); // configure ARM A9 private timer
    config_PS2();  // configure PS/2 port to generate interrupts

    enable_A9_interrupts(); // enable interrupts

    while (1) {
        while (!timeout)
            ; // wait to synchronize with timer

        /* display PS/2 data (from interrupt service routine) on HEX displays */
        HEX_PS2(byte1, byte2, byte3);
        timeout = 0;
        if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x74)) printf("\n Derecha");
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x72)) printf("\n Abajo");
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x75)) printf("\n Arriba");
	    else if((byte1==0xE0)&&(byte2==0xF0)&&(byte3==0x6B)) printf("\n Izquierda");
    }
}

/* setup private timer in the ARM A9 */
void config_MPcore_private_timer() {
    volatile int * MPcore_private_timer_ptr =
        (int *)MPCORE_PRIV_TIMER; // timer base address

    /* set the timer period */
    int counter = 20000000; // period = 1/(200 MHz) x 40x10^6 = 0.2 sec
    *(MPcore_private_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(MPcore_private_timer_ptr + 2) = 0x7; // int mask = 1, mode = 1, enable = 1
}

/* setup the PS/2 interrupts */
void config_PS2() {
    volatile int * PS2_ptr = (int *)PS2_BASE; // PS/2 port address

    *(PS2_ptr) = 0xFF; /* reset */
    *(PS2_ptr + 1) = 0x1; /* write to the PS/2 Control register to enable interrupts */
}

/****************************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
****************************************************************************************/
void HEX_PS2(char b1, char b2, char b3) {
    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display in the DE2 Media Computer, for the hex digits 0 -
     * F */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int  shift_buffer, nibble;
    unsigned char code;
    int           i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i) {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code   = seven_seg_decode_table[nibble];
        hex_segs[i]  = code;
        shift_buffer = shift_buffer >> 4;
    }
    /* drive the hex displays */
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}
