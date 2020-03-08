# **Lab 4**

Read the lab4.pdf to get more details on each part of the project.

This lab is done in assembly language.

# **Part 1**

Write an ARM assembly language program that displays a decimal digit on the seven-segment display and make use of the pushbuttons.

If KEY0 is pressed on the DE1-SoC board, you should set the number displayed on HEX0 to 0. If KEY1 is 2 
pressed then increment the displayed number, and if KEY2 is pressed then decrement the number. Pressing
KEY3 should blank the display, and pressing any other KEY after that should return the display to 0.

# **Part 2**

Write an ARM assembly language program that displays a two-digit decimal counter on the seven-segment
displays HEX1 − 0. The counter should be incremented approximately every 0.25 seconds. When the
counter reaches the value 99, it should start again at 0. The counter should stop/start when any pushbutton
KEY is pressed.

# **Part 3**

In Part 2 you used a delay loop to cause the ARM processor to wait for approximately 0.25 seconds.
The processor loaded a large value into a register before the loop, and then decremented that value until it
reached 0. In this part you are to modify your code so that a hardware timer is used to measure an exact
delay of 0.25 seconds. You should use polled I/O to cause the ARM processor to wait for the timer.

# **Part 4**

In this part you are to write an assembly language program that implements a real-time clock. Display
the time on the seven-segment displays HEX3 − 0 in the format SS:DD, where SS are seconds and DD are
hundredths of a second. Measure time intervals of 0.01 seconds in your program by using polled I/O with
the A9 Private Timer. You should be able to stop/run the clock by pressing any pushbutton KEY. When the
clock reaches 59:99, it should wrap around to 00:00.
