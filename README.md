Snake
=====

Sample project implementing the game snake using microcontroller boards and LCD display

1. Required Hardware
--------------------
For this project you will need: 

	* 2 - MC52259 V2 ColdFire Microcontroller boards
	
	* 1 - Freescale Tower System
	
	* 1 - LCD display board
	
2. Assembling Hardware
----------------------
In order to assemble the hardware components correctly, you will first want to take the Freescale
Tower System. The Tower System is made up of two pieces with multiple slots on each piece. At the top slot,
snap the LCD display into place.  In the bottom slot, snap one of the MC52259 Microcontroller boards into place. 
The second MC52259 microcontroller board will be used as a wireless controller for the game.
	
3. Install Classic Codewarrior
------------------------------
If running 32-bit Windows 7 or earlier OS, the Classic Codewarrior can be installed directly because Classic Codewarrior
was released before Windows 7. However, if installing Classic Codewarrior on 64-bit Windows 7 or later, install the program 
using Microsoft Windows XP mode.

Classic Codewarrior can be dowloaded at Freescale website: http://www.freescale.com/webapp/sps/site/overview.jsp?code=CW_DOWNLOADS


4. Install Program on Microcontroller board
-------------------------------------------
In order to run the program, we must first compile the program in Codewarrior and create a binary file. Using a USB connector 
cable given with our microcontroller board we can then connect the board to our computer.  Next install a utility
program so that we can open the binary file.  Select the microcontroller board as the target of our binary file and program
the board. 

5. Running the Program
-----------------------
In order to run the board keep the microcontroller plugged into the USB port on your computer in order to power the entire system.
Next, switch the microcontroller board on and the splash screen should appear on the LCD.  From there, you can use the joystick located 
next to the LCD to control the game and Snake. 