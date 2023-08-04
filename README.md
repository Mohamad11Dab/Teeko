# Teeko

Game of Teeko using the AVR ATmega324A microcontroller which runs the program using The Microchip Studio environment and receives input from a number of sources and outputs a display to an LED matrix with additional information being output to a serial terminal and a seven segment display and other devices

This project involves creating a replica of the board game ‘Teeko’. Teeko is a turn-based game 
played between two players on a 5x5 board. Each player has a set of 4 coloured pieces (green for 
player 1, red for player 2) they must take turns placing on the board. A player wins the game if 
they manage to place all of their pieces in a line (which may be a diagonal, vertical or horizontal 
line).

• project.c – this is the main file that contains the event loop and examples of how timebased events are implemented.
• game.h/game.c – this file contains the implementation of the board used to store the state 
of the game and the position of the cursor. 
• display.h/display.c – this file contains the implementation for displaying the current state 
of the board. 
• buttons.h/buttons.c – this contains the code which deals with the push buttons. It sets up 
pin change interrupts on those pins and records rising edges (buttons being pushed). 
• ledmatrix.h/ledmatrix.c  – this contains functions which give easier 
access to the services provided by the LED matrix. It makes use of the SPI routines 
implemented in spi.c
• pixel_colour.h  – this file contains definitions of some useful colours. 
• serialio.h/serialio.c – this file is responsible for handling serial input and output using 
interrupts. It also maps the C standard IO routines (e.g. printf() and fgetc()) to use the serial 
interface 
• spi.h/spi.c  – this file encapsulates all SPI communication.
• terminalio.h/terminalio.c – this encapsulates the sending of various escape sequences
which enable some control over terminal appearance and text placement.
• timer0.h/timer0.c – sets up a timer that is used to generate an interrupt every millisecond 
and update a global time value that is used to time various game events