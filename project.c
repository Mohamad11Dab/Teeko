/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett
 * Modified by <YOUR NAME HERE>
 */ 

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#define F_CPU 8000000L
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"



// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

/////////////////////////////// main //////////////////////////////////
int main(void) {
 
DDRA = 0xFF;
DDRC = 0xFF;



	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	start_screen();
	 
	
	
	// Loop forever,

	while(1) {
		
		new_game();
		
		play_game();
		
		handle_game_over();
	}


}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	// Clear terminal screen and output a message

	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Teeko"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by Mohamad Dabboussi"));
	move_terminal_cursor(10,13);
	printf_P(PSTR("Student number : s4695069"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		if (serial_input == 'q' || serial_input == 'Q') {
			sound=1-sound;
		}
		
		
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
	
	
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	initialise_game();
	starttup();
	PORTA |= (1<<0);
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time;
	uint8_t btn; //the button pushed
	
	last_flash_time = get_current_time();
	
    move_terminal_cursor(10,8);
    printf("Current Player: 1 (Green)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
	// We play the game until it's over
	while(!is_game_over()) {
				switch_digits();
//digit=digit-1;
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
        
		btn = button_pushed();
		
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 'w' || serial_input == 'W') {
			move_display_cursor(0,1);
			//cursormove();
		 
			
		}
		if (serial_input == 'q' || serial_input == 'Q') {
			sound=1-sound;
		}
		if (serial_input == 'a' || serial_input == 'A') {
			move_display_cursor(-1,0);
			//cursormove();
		}
		if (serial_input == 's' || serial_input == 'S') {
			
			move_display_cursor(0,-1);
			//cursormove();
		}
		if (serial_input == 'd' || serial_input == 'D') {
			
			move_display_cursor(1,0);
			//cursormove();
		}
        if (serial_input == 'p' || serial_input == 'P') {
	        while(1){
				switch_digits();
btn=button_pushed();
char serial_input = -1;
if (serial_input_available()) {
	serial_input = fgetc(stdin);
if (serial_input == 'p' || serial_input == 'P') {
	
	break;
}
}


}
        }
		if (serial_input == ' ') {
			piece_placement();
           // detects2();
           // detects1();

		}
		
		if (btn == BUTTON3_PUSHED) {
			// If button 3 is pushed, move left,
			// i.e decrease x by 1 and leave y the same
			move_display_cursor(-1, 0);
		}
		if (btn == BUTTON2_PUSHED) {
			// If button 2 is pushed, move right,
			// i.e increase x by 1 and leave y the same
			move_display_cursor(1, 0);
		}
		if (btn == BUTTON1_PUSHED) {
			// If button 1 is pushed, move up,
			// i.e increase y by 1 and leave x the same
			move_display_cursor(0,1);
		}
		if (btn == BUTTON0_PUSHED) {
			// If button 0 is pushed, move down,
			// i.e decrease y by 1 and leave x the same
			move_display_cursor(0,-1);
		}

	
		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_cursor();
			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
		}
		switch_digits();
	}
	// We get here if the game is over.
}

void handle_game_over() {
 endup();
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == NO_BUTTON_PUSHED) {
		switch_digits();
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 'q' || serial_input == 'Q') {
			sound=1-sound;
			
			
		}
	}
	
}
