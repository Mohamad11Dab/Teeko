/*
 * game.c
 *
 * Contains functions relating to the play of the game Teeko
 *
 * Authors: Luke Kamols, Jarrod Bennett
 */ 
#define __DELAY_BACKWARD_COMPATIBLE__
#include "timer0.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "terminalio.h"
#include <avr/io.h>
#include <util/delay.h>



// Start pieces in the middle of the board
#define CURSOR_X_START ((int)(WIDTH/2))
#define CURSOR_Y_START ((int)(HEIGHT/2))
#define SPEAKER_PORT PORTD
#define SPEAKER_DDR DDRD
#define SPEAKER_PIN 4

uint8_t board[WIDTH][HEIGHT];
// cursor coordinates should be /* SIGNED */ to allow left and down movement.
// All other positions should be unsigned as there are no negative coordinates.
int8_t cursor_x;
int8_t cursor_y;
int8_t initial_cursor_x;
int8_t initial_cursor_y;
int8_t switcher;
int8_t switcher2;
uint8_t cursor_visible;
uint8_t current_player;
uint8_t count_pieces ; // there should be only 8 pieces on the board
uint8_t did_get_piece;
int8_t player1_won;
int8_t player2_won;
uint8_t seven_seg[5] = { 63,6,91,79,102};
uint8_t digit;
uint8_t value;
uint8_t sound=1;


void intialize_winning(void){
player1_won=0;
player2_won=0;
//sound=1;
}


void initialise_game(void) {
	
	// initialize the display we are using
	initialise_display();
	
	// initialize the board to be all empty
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			board[x][y] = EMPTY_SQUARE;
		}
	}
	
	// set the starting player
	current_player = PLAYER_1;

	// also set where the cursor starts
	cursor_x = CURSOR_X_START;
	cursor_y = CURSOR_Y_START;
	cursor_visible = 0;
    count_pieces=0;
switcher=2;
switcher2=0;
did_get_piece=0;
digit=1;



}


void PLAYNOTE(float duration,float frequency){
	long int i,cycles;
	float half_period;
	float wavelength;
	
	wavelength=(1/frequency)*1000; 
	cycles=duration/wavelength; 
	half_period = wavelength/2; 
	SPEAKER_DDR |= (1 << SPEAKER_PIN);
	for (i=0;i<cycles;i++){
		_delay_ms(half_period); 
		SPEAKER_PORT |= (1 << SPEAKER_PIN);
		_delay_ms(half_period);
		SPEAKER_PORT &= ~(1 << SPEAKER_PIN); 
	}
}

void starttup(void){
	
	if(sound){
	PLAYNOTE(400,880);  // Musical note 880 Hz
  PLAYNOTE(400,932);
	  PLAYNOTE(400,988);
	  PLAYNOTE(400,1047);
	  PLAYNOTE(400,1109);
	  PLAYNOTE(400,1175);
	  PLAYNOTE(400,1244);
	  PLAYNOTE(400,1319);
	  PLAYNOTE(400,1397);
	  PLAYNOTE(400,1480);
	  PLAYNOTE(400,1568);
	  PLAYNOTE(400,1660);
	}
}
void endup(void){
	if(sound){
	PLAYNOTE(400,1660);
	PLAYNOTE(400,1568);
	PLAYNOTE(400,1480);
	PLAYNOTE(400,1397);
	PLAYNOTE(400,1319);
	PLAYNOTE(400,1244);
	PLAYNOTE(400,1175);
	PLAYNOTE(400,1109);
	PLAYNOTE(400,1047);
	PLAYNOTE(400,988);
	PLAYNOTE(400,932);
}
}

void cursormove(void){
	if (sound){
		PLAYNOTE(400,500);
		PLAYNOTE(400,700);
		PLAYNOTE(400,500);
		//PLAYNOTE(400,988);
		
	}
}




uint8_t get_piece_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= WIDTH) {
		return EMPTY_SQUARE;
	} else {
		//if in the bounds, just index into the array
		return board[x][y];
	}
}

void flash_cursor(void) {
	
	if (cursor_visible) {
		// we need to flash the cursor off, it should be replaced by
		// the colour of the piece which is at that location
		uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
		update_square_colour(cursor_x, cursor_y, piece_at_cursor);
		
	} else {
		// we need to flash the cursor on
		update_square_colour(cursor_x, cursor_y, CURSOR);
	}
	cursor_visible = 1 - cursor_visible; //alternate between 0 and 1
}

//check the header file game.h for a description of what this function should do
// (it may contain some hints as to how to move the cursor)
void move_display_cursor(int8_t dx, int8_t dy) {

	cursor_visible = 1;
	uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
	update_square_colour(cursor_x, cursor_y, piece_at_cursor);

	if ((cursor_x + dx) % WIDTH < 0 ){
		dx=4;
		cursor_x = (cursor_x + dx) % WIDTH;
	}else {
	cursor_x = (cursor_x + dx) % WIDTH;
	}
	if ((cursor_y + dy) % HEIGHT < 0) {
		dy=4;
		cursor_y = (cursor_y + dy) % HEIGHT;
	} else {
	cursor_y = (cursor_y + dy) % HEIGHT;
	}
	cursor_visible = 0;
	update_square_colour(cursor_x, cursor_y, CURSOR);
if(count_pieces<8){
	if (board[cursor_x][cursor_y] == EMPTY_SQUARE){
		PORTA |=(1<<0);
		}else {
		PORTA &=~(1<<0);
	}
}

if(switcher==0){
	if (current_player == PLAYER_1) {
		if(board[cursor_x][cursor_y]==PLAYER_1){
			PORTA |=(1<<0);

			}else{PORTA &= ~(1<<0);}
	} else if (current_player == PLAYER_2){
			if (board[cursor_x][cursor_y]==PLAYER_2){
				PORTA |=(1<<0);
				}else{PORTA &= ~(1<<0);}
			}
}


if(switcher == 1){

	if (current_player==PLAYER_1){
PORTA &= ~(1<<0);
		for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
			for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
				if (cursor_x==i && cursor_y==j && board[i][j]==POSSIBLE && (cursor_x !=initial_cursor_x || cursor_y != initial_cursor_y)){
					PORTA |=(1<<0);


				}
			}
		}


		
		} else if (current_player==PLAYER_2){
PORTA &= ~(1<<0);
		for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
			for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
				if (cursor_x==i && cursor_y==j && board[i][j]==POSSIBLE && (cursor_x !=initial_cursor_x || cursor_y != initial_cursor_y)){
					PORTA |=(1<<0);

				}
		}

		
	}

}
}





    
	
	
	//YOUR CODE HERE
	/*suggestions for implementation:
	 * 1: remove the display of the cursor at the current location
	 *		(and replace it with whatever piece is at that location)
	 * 2: update the positional knowledge of the cursor, this will include
	 *		variables cursor_x, cursor_y and cursor_visible. Make sure you
	 *		consider what should happen if the cursor moves off the board.
	 * 3: display the cursor at the new location
	 * 4: reset the cursor flashing cycle. See project.c for how the cursor
	 *		is flashed.
	 */
 
	
}
void piece_placement(void) {
uint32_t last_flash_time, current_time;

  while (count_pieces < 8){
	if (current_player == PLAYER_1) {
		clear_terminal();
		move_terminal_cursor(10,8);
		printf("Current Player: 1 (Green)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
	} else if (current_player == PLAYER_2) {
		clear_terminal();
		move_terminal_cursor(10,8);
		printf("Current Player: 2 (Red)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
	}
    
	if (board[cursor_x][cursor_y] == EMPTY_SQUARE ) {
		board[cursor_x][cursor_y] = current_player;
	   update_square_colour(cursor_x, cursor_y, current_player);
PORTA &= ~(1<<0);
	} else {
		break;
	}
     count_pieces++;
	if (current_player == PLAYER_1) {
		current_player = PLAYER_2;
	} else if (current_player == PLAYER_2) {
		current_player = PLAYER_1;
	}
 }

if (count_pieces ==8){
for(int8_t h=0;h<5;h++){
for(int8_t m=0;m<5;m++){
 if (board[h][m]== PLAYER_1 || board[h][m]== PLAYER_2){
update_square_colour(h,m,POSSIBLE);
}
}
}
last_flash_time=get_current_time();
while(1){
current_time=get_current_time();
if(current_time >= last_flash_time + 500){

for(int8_t x=0;x<5;x++){
	for(int8_t y=0;y<5;y++){
		if (board[x][y]== PLAYER_1){
			update_square_colour(x,y,PLAYER_1);
		} else if(board[x][y]== PLAYER_2){
update_square_colour(x,y,PLAYER_2);
}
	}
}
break;


}
}


}




 
switcher2=1;
	


while(switcher==0){
    if (current_player == PLAYER_1) {
   if(board[cursor_x][cursor_y]==PLAYER_1){
  //board[cursor_x][cursor_y]=EMPTY_SQUARE;
  //update_square_colour(cursor_x,cursor_y,EMPTY_SQUARE);
  initial_cursor_x=cursor_x;
  initial_cursor_y=cursor_y;

for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
	for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
		if(board[i][j]==EMPTY_SQUARE && i>=0 && i<5 && j>=0 && j<5){
			update_square_colour(i,j,POSSIBLE);
board[i][j]=POSSIBLE;

		}

	}
}
cursormove();
update_square_colour(cursor_x,cursor_y,EMPTY_SQUARE);
board[cursor_x][cursor_y]=EMPTY_SQUARE;
  did_get_piece=1;
  switcher2=0;
PORTA &= ~(1<<0);
} else {
     break;
  }

} else if (current_player == PLAYER_2) {
  if(board[cursor_x][cursor_y]== PLAYER_2){
	//update_square_colour(cursor_x,cursor_y,EMPTY_SQUARE);
   // board[cursor_x][cursor_y]=EMPTY_SQUARE;
	initial_cursor_x=cursor_x;
	initial_cursor_y=cursor_y;

for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
	for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
if(board[i][j]==EMPTY_SQUARE && i>=0 && i<5 && j>=0 && j<5){
update_square_colour(i,j,POSSIBLE);
board[i][j]=POSSIBLE;
}

}
}


cursormove();
update_square_colour(cursor_x,cursor_y,EMPTY_SQUARE);
    board[cursor_x][cursor_y]=EMPTY_SQUARE;
     did_get_piece=1;
    switcher2=0;
PORTA &=~(1<<0);
} else{
  break;
} 
}

 else {break;}
}






while(switcher == 1 ){
if (current_player==PLAYER_1){
 for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
    for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
     if (cursor_x==i && cursor_y==j && board[i][j]==POSSIBLE && (cursor_x !=initial_cursor_x || cursor_y != initial_cursor_y)){
    for(int8_t k=0; k<5;k++){
      for(int8_t l=0; l<5;l++){
if(board[k][l] == POSSIBLE){
board[k][l]=EMPTY_SQUARE;
update_square_colour(k,l,EMPTY_SQUARE);
}
}


}
    update_square_colour(cursor_x,cursor_y,PLAYER_1);
    board[cursor_x][cursor_y]= PLAYER_1;
     switcher2=0;
     did_get_piece=0;
    current_player=PLAYER_2;
clear_terminal();
move_terminal_cursor(10,8);
printf("Player 2's turn to move (Red)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
PORTA &= ~(1<<0);

}
}
}

break;
} else if (current_player==PLAYER_2){
for(int8_t i=initial_cursor_x-1; i<initial_cursor_x+2;i++){
	for(int8_t j=initial_cursor_y-1; j<initial_cursor_y+2;j++){
		if (cursor_x==i && cursor_y==j && board[i][j]==POSSIBLE && (cursor_x !=initial_cursor_x || cursor_y != initial_cursor_y)){
for(int8_t k=0; k<5;k++){
	for(int8_t l=0; l<5;l++){
		if(board[k][l] == POSSIBLE){
			board[k][l]=EMPTY_SQUARE;
			update_square_colour(k,l,EMPTY_SQUARE);
		}
	}


}
			update_square_colour(cursor_x,cursor_y,PLAYER_2);
			board[cursor_x][cursor_y]= PLAYER_2;
            switcher2=0;
did_get_piece=0;
            current_player=PLAYER_1;
clear_terminal();
move_terminal_cursor(10,8);
printf("Player 1's turn to move (Green)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
PORTA &= ~(1<<0);
}




}
}

break;
}
}

if(switcher2==0){
switcher=1-switcher;
}

while (count_pieces == 8){
	clear_terminal();
	move_terminal_cursor(10,8);
	printf("Player 1's turn to move (Green)");
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
	switcher=0;
	count_pieces++;
	
}



}

uint8_t is_game_over(void) {











uint32_t last_flash_time, current_time;
	 int8_t game_over=0;
   if(current_player == PLAYER_2){
   for(int8_t j =0; j<HEIGHT; j++){
   if( board[1][j]== PLAYER_1 && board[2][j]== PLAYER_1 && board[3][j]== PLAYER_1 && (board[0][j]==PLAYER_1 || board[4][j]== PLAYER_1)){
    game_over=1;
    player1_won=player1_won+1;
    clear_terminal();
    move_terminal_cursor(10,10);
    printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
    

for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
					} 
			}
		}
		break;


	}
}




     
}
}

for(int8_t i =0; i<WIDTH; i++){
	if( board[i][1]== PLAYER_1 && board[i][2]== PLAYER_1 && board[i][3]== PLAYER_1 && (board[i][0]==PLAYER_1 || board[i][4]== PLAYER_1)){
		game_over=1;
        player1_won=player1_won+1;
		clear_terminal();
		move_terminal_cursor(10,10);
		printf("Player 1 has won : %d",player1_won);
		move_terminal_cursor(10,11);
		printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
				}
			}
		}
		break;


	}
}
		
	}
}

if((board[4][3]==PLAYER_1 && board[3][2]==PLAYER_1 && board[2][1]==PLAYER_1 && board[1][0]==PLAYER_1 ) ||(board[3][4]==PLAYER_1 && board[2][3]==PLAYER_1 && board[1][2]==PLAYER_1 && board[0][1]==PLAYER_1)){
game_over=1;
player1_won=player1_won+1;
clear_terminal();
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
				}
			}
		}
		break;


	}
}
}
if (board[3][3]==PLAYER_1 && board[2][2]==PLAYER_1 && board[1][1]==PLAYER_1 && (board[4][4]==PLAYER_1 || board[0][0]==PLAYER_1 )){
game_over=1;
player1_won=player1_won+1;
clear_terminal();
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
				}
			}
		}
		break;


	}
}
}

if((board[0][3]==PLAYER_1 && board[1][2]==PLAYER_1 && board[2][1]==PLAYER_1 && board[3][0]==PLAYER_1 ) ||(board[1][4]==PLAYER_1 && board[2][3]==PLAYER_1 && board[3][2]==PLAYER_1 && board[4][1]==PLAYER_1)){
	game_over=1;
	player1_won=player1_won+1;
	clear_terminal();
	move_terminal_cursor(10,10);
	printf("Player 1 has won : %d",player1_won);
	move_terminal_cursor(10,11);
	printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
				}
			}
		}
		break;


	}
}
}
if (board[1][3]==PLAYER_1 && board[2][2]==PLAYER_1 && board[3][1]==PLAYER_1 && (board[0][4]==PLAYER_1 || board[4][0]==PLAYER_1 )){
	game_over=1;
	player1_won=player1_won+1;
	clear_terminal();
	move_terminal_cursor(10,10);
	printf("Player 1 has won : %d",player1_won);
	move_terminal_cursor(10,11);
	printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_1){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_1){
					update_square_colour(x,y,PLAYER_1);
				}
			}
		}
		break;


	}
}
}





} else if (current_player == PLAYER_1){
for(int8_t j =0; j<HEIGHT; j++){
	if( board[1][j]== PLAYER_2 && board[2][j]== PLAYER_2 && board[3][j]== PLAYER_2 && (board[0][j]==PLAYER_2 || board[4][j]== PLAYER_2)){
		game_over=1;
		player2_won=player2_won+1;
		clear_terminal();
		move_terminal_cursor(10,10);
		printf("Player 1 has won : %d",player1_won);
		move_terminal_cursor(10,11);
		printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}







		
	}
}


for(int8_t i =0; i<WIDTH; i++){
	if( board[i][1]== PLAYER_2 && board[i][2]== PLAYER_2 && board[i][3]== PLAYER_2 && (board[i][0]==PLAYER_2 || board[i][4]== PLAYER_2)){
		game_over=1;
		player2_won=player2_won+1;
		clear_terminal();
		move_terminal_cursor(10,10);
		printf("Player 1 has won : %d",player1_won);
		move_terminal_cursor(10,11);
		printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}
		
	}
}
if((board[4][3]==PLAYER_2 && board[3][2]==PLAYER_2 && board[2][1]==PLAYER_2 && board[1][0]==PLAYER_2 ) ||(board[3][4]==PLAYER_2 && board[2][3]==PLAYER_2 && board[1][2]==PLAYER_2 && board[0][1]==PLAYER_2)){
	game_over=1;
player2_won=player2_won+1;
clear_terminal();
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}

}
if (board[3][3]==PLAYER_2 && board[2][2]==PLAYER_2 && board[1][1]==PLAYER_2 && (board[4][4]==PLAYER_2 || board[0][0]==PLAYER_2 )){
	game_over=1;
player2_won=player2_won+1;
clear_terminal();
move_terminal_cursor(10,10);
printf("Player 1 has won : %d",player1_won);
move_terminal_cursor(10,11);
printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}

}

if((board[0][3]==PLAYER_2 && board[1][2]==PLAYER_2 && board[2][1]==PLAYER_2 && board[3][0]==PLAYER_2 ) ||(board[1][4]==PLAYER_2 && board[2][3]==PLAYER_2 && board[3][2]==PLAYER_2 && board[4][1]==PLAYER_2)){
	game_over=1;
	player2_won=player2_won+1;
	clear_terminal();
	move_terminal_cursor(10,10);
	printf("Player 1 has won : %d",player1_won);
	move_terminal_cursor(10,11);
	printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}
}
if (board[1][3]==PLAYER_2 && board[2][2]==PLAYER_2 && board[3][1]==PLAYER_2 && (board[0][4]==PLAYER_2 || board[4][0]==PLAYER_2 )){
	game_over=1;
	player2_won=player2_won+1;
	clear_terminal();
	move_terminal_cursor(10,10);
	printf("Player 1 has won : %d",player1_won);
	move_terminal_cursor(10,11);
	printf("Player 2 has won : %d",player2_won);
for(int8_t h=0;h<5;h++){
	for(int8_t m=0;m<5;m++){
		if (board[h][m]== PLAYER_2){
			update_square_colour(h,m,POSSIBLE);
		}
	}
}
last_flash_time=get_current_time();
while(1){
	current_time=get_current_time();
	if(current_time >= last_flash_time + 500){

		for(int8_t x=0;x<5;x++){
			for(int8_t y=0;y<5;y++){
				if (board[x][y]== PLAYER_2){
					update_square_colour(x,y,PLAYER_2);
				}
			}
		}
		break;


	}
}
}


}
if(player1_won==2){

move_terminal_cursor(10,9);
printf("PLAYER 1 IS THE FINAL WINNER IN BEST OF 3 TOURNAMENT!");

player2_won=0;
player1_won=0;

}
if(player2_won==2){
	
	move_terminal_cursor(10,9);
	printf("PLAYER 2 IS THE FINAL WINNER IN BEST OF 3 TOURNAMENT!");
	
	player2_won=0;
	player1_won=0;

}


	return game_over;
}



uint8_t detects1(void){

int8_t count;
int8_t big_number;

big_number=0;



count=0;


 for(int8_t i=0; i<5;i++){
  for (int8_t j=0; j<5;j++){
if (j==0){
count=0;
}
  
  if(board[i][j]==PLAYER_1){
  count++;
  if(j==4) {

if(big_number<=count){
	big_number=count;
	count=0;
}

}

  

} else {
if(big_number<=count){
big_number=count;
count=0;
}
}

}

}

int8_t horizontal ;
horizontal=big_number;
count=0;
big_number=0;

for(int8_t i=0; i<5;i++){
	count=0;
	for (int8_t j=0; j<5;j++){
if (j==0){
	count=0;
}
		
		if(board[j][i]==PLAYER_1){
			count++;
			if( j==4) {

				if(big_number<=count){
					big_number=count;
					count=0;
				}

			}

			

			} else {
			if(big_number<=count){
				big_number=count;
				count=0;
			}
		}

	}

}

int8_t which_bigger;
if(big_number<=horizontal){
which_bigger=horizontal;
}else {
which_bigger=big_number;
}
big_number=0;
count =0;

uint8_t diagonal;
diagonal=0;
if(board[1][4]==PLAYER_1 && board[0][3]== PLAYER_1){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
int8_t j=2;
for(int8_t i=0; i<3;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
	}else{
		if(diagonal>big_number){
	big_number=diagonal;
	
}
	diagonal=0;	
	}
	j++;
} 
if(diagonal>big_number){
	big_number=diagonal;
}


diagonal=0;
 j=1;
for(int8_t i=0; i<4;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
 j=0;
for(int8_t i=0; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
 j=0;
for(int8_t i=1; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
 j=0;
for(int8_t i=2; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
if(board[3][0]== PLAYER_1 && board[4][1]==PLAYER_1){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
if(board[4][3]==PLAYER_1 && board[3][4]== PLAYER_1){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
j=4;
for(int8_t i=2; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=4;
for(int8_t i=1; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=4;
for(int8_t i=0; i<5;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=3;
for(int8_t i=0; i<4;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=2;
for(int8_t i=0; i<3;i++) {
	if (board[i][j]==PLAYER_1){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
if(board[0][1]==PLAYER_1 && board[1][0]== PLAYER_1){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
if(big_number>which_bigger){
	which_bigger=big_number;
}






return which_bigger;



}
uint8_t detects2(void){

int8_t count;
int8_t big_number;

big_number=0;



count=0;


for(int8_t i=0; i<5;i++){
	for (int8_t j=0; j<5;j++){
		if (j==0){
			count=0;
		}
		
		if(board[i][j]==PLAYER_2){
			count++;
			if( j==4) {

				if(big_number<=count){
					big_number=count;
					count=0;
				}

			}

			

			} else {
			if(big_number<=count){
				big_number=count;
				count=0;
			}
		}

	}

}

int8_t horizontal ;
horizontal=big_number;
count=0;
big_number=0;

for(int8_t i=0; i<5;i++){
	count=0;
	for (int8_t j=0; j<5;j++){
		if (j==0){
			count=0;
		}
		
		if(board[j][i]==PLAYER_2){
			count++;
			if( j==4) {

				if(big_number<=count){
					big_number=count;
					count=0;
				}

			}

			

			} else {
			if(big_number<=count){
				big_number=count;
				count=0;
			}
		}

	}

}

int8_t which_bigger;
if(big_number<=horizontal){
	which_bigger=horizontal;
	}else {
	which_bigger=big_number;
}
big_number=0;
count =0;

uint8_t diagonal;
diagonal=0;
if(board[1][4]==PLAYER_2 && board[0][3]== PLAYER_2){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
int8_t j=2;
for(int8_t i=0; i<3;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}


diagonal=0;
j=1;
for(int8_t i=0; i<4;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=0;
for(int8_t i=0; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=0;
for(int8_t i=1; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=0;
for(int8_t i=2; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j++;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
if(board[3][0]== PLAYER_2 && board[4][1]==PLAYER_2){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
if(board[4][3]==PLAYER_2 && board[3][4]== PLAYER_2){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
diagonal=0;
j=4;
for(int8_t i=2; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=4;
for(int8_t i=1; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=4;
for(int8_t i=0; i<5;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=3;
for(int8_t i=0; i<4;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
j=2;
for(int8_t i=0; i<3;i++) {
	if (board[i][j]==PLAYER_2){
		diagonal++;
		}else{
		if(diagonal>big_number){
			big_number=diagonal;
			
		}
		diagonal=0;
	}
	j--;
}
if(diagonal>big_number){
	big_number=diagonal;
}
diagonal=0;
if(board[0][1]==PLAYER_2 && board[1][0]== PLAYER_2){
	diagonal=2;
	if(diagonal>big_number){
		big_number=diagonal;
	}
}
if(big_number>which_bigger){
	which_bigger=big_number;
}








return which_bigger;




}

void switch_digits(void){


if (digit==0){
value=detects2();
PORTA &= ~(1<<1);

} else if (digit == 1){
value = detects1();
PORTA |= (digit<<1);
}
//PORTA = (digit<<1);
PORTC=seven_seg[value];
digit=1-digit;
}