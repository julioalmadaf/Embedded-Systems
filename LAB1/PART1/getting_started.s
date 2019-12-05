.include    "address_map_arm.s" 

/********************************************************************************
 * This program demonstrates use of parallel ports in the Computer System
 *
 * It performs the following:
 * 	1. displays a rotating pattern on the LEDs
 * 	2. if a KEY is pressed, uses SW switches as the pattern
 ********************************************************************************/
.text        /* executable code follows */
.global     _start 
_start:                         

        MOV     R0, #31         // used to rotate a bit pattern: 31 positions to the
                                // right is equivalent to 1 position to the left
        LDR     R1, =LED_BASE   // base address of LED lights
        LDR     R2, =SW_BASE    // base address of SW switches
        LDR     R3, =KEY_BASE   // base address of KEY pushbuttons
        LDR     R4, LED_bits    

DO_DISPLAY:                     
        LDR     R5, [R2]        // load SW switches

        LDR     R6, [R3]        // load pushbutton keys
        CMP     R6, #0          // check if any key is presssed
        BEQ     NO_BUTTON       

        MOV     R4, R5          // copy SW switch values onto LED displays
        ROR     R5, R5, #8      // the SW values are copied into the upper three
                                // bytes of the pattern register
        ORR     R4, R4, R5      // needed to make pattern consistent as all 32-bits
                                // of a register are rotated
        ROR     R5, R5, #8      // but only the lowest 8-bits are displayed on LEDs
        ORR     R4, R4, R5      
        ROR     R5, R5, #8      
        ORR     R4, R4, R5      
WAIT:                           
        LDR     R6, [R3]        // load pushbuttons
        CMP     R6, #0          
        BNE     WAIT            // wait for button release

NO_BUTTON:                      
        STR     R4, [R1]        // store pattern to the LED displays
        ROR     R4, R0          // rotate the displayed pattern to the left

        LDR     R6, =50000000   // delay counter
DELAY:                          
        SUBS    R6, R6, #1      
        BNE     DELAY           

        B       DO_DISPLAY      


LED_bits:                       
.word       0x0F0F0F0F 

.end         
