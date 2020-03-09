# **Lab 7**

Read the lab7.pdf to get more details on each part of the project.

This lab is done in C language.

# **Part 1**

The purpose of your program is to show the numbers 0 to 3 on the HEX0 to HEX3 displays, respectively,
when a corresponding pushbutton KEY is pressed. Since the main program only idles in a loop, you have
to control the displays by using an interrupt service routine for the pushbutton KEYs port.You have to write the code for the pushbutton isr() interrupt service routine. Your code should show
the digit 0 on the HEX0 display when KEY0 is pressed, and then if KEY0 is pressed again the display
should be “blank”. You should toggle the HEX0 display between 0 and “blank” in this manner each
time KEY0 is pressed. Similarly, toggle between “blank” and 1, 2, or 3 on the HEX1 to HEX3 displays
each time KEY1, KEY2, or KEY3 is pressed, respectively.

# **Part 2**

The program executes an endless loop writing the value of the global variable count to the
red lights LEDR. In the interrupt service routine for HPS Timer 0 you are to increment the variable count
by the value of the run global variable, which should be either 1 or 0. You are to toggle the value of the run
global variable in the interrupt service routine for the pushbutton KEYs, each time a KEY is pressed. When
run = 0, the main program will display a static count on the red lights, and when run = 1, the count shown
on the red lights will increment every 0.25 seconds.

# **Part 3**
Modify your program from Part 2 so that you can vary the speed at which the counter displayed on the
red lights is incremented. All of your changes for this part should be made in the interrupt service routine
for the pushbutton KEYs. The main program and the rest of your code should not be changed.
Implement the following behavior. When KEY0 is pressed, the value of the run variable should be toggled,
as in Part I. Hence, pressing KEY0 stops/runs the incrementing of the count variable. When KEY1 is pressed,
the rate at which the count variable is incremented should be doubled, and when KEY2 is pressed the rate
should be halved. You should implement this feature by stopping HPS Timer 0 within the pushbutton KEYs
interrupt service routine, modifying the load value used in the timer, and then restarting the timer.

# **Part 4**
For this part you are to add a third source of interrupts to your program, using the A9 Private Timer.
Set up the timer to provide an interrupt every 1/100 of a second. Use this timer to increment a global vari-
able called time. You should use the time variable as a real-time clock that is shown on the seven-segment
displays HEX5 − 0. Use the format MM:SS:DD, where MM are minutes, SS are seconds and DD are hun-
dredths of a second. You should be able to stop/run the clock by pressing pushbutton KEY3. When the clock
reaches 59:59:99, it should wrap around to 00:00:00.


