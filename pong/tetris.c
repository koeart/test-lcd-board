#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "font8x6.h"
#include "main.h"
#include "lcd.h"
#include "timer.h"
#include "tetris.h"
#include "pong.h"

/* Tetris
 * 
 * 
 * 
 * 
 * */
//extern rechterTeil()	0xF0

uint8_t stones[6] = {0x0F, 0x33, 0x63, 0x36, 0x71, 0x17}; 		//Stick, square, flash left, flash right, L right, L left
uint8_t stone_xpos = 23;
uint8_t stone_ypos = 0;
uint8_t *stone;
uint8_t game = 1;
uint8_t points = 0;
enum rotation stone_rotation = zero;

void new_stone(void) {
	uint8_t i = 0;
	i = rand() % 6;
	stone = &stones[i];
	}

void tetris() {
	ausrichtung = rechts;
	if (debug)	{
		lcd_write_number_u(ausrichtung);
		wait_ms(5000);
	}
	
	for(uint8_t count = 0; count < 40; count++) {
		
		lcd_cls();
		uint8_t x, y;
		x=23;
		y=0;
		new_stone();
		lcd_print_hex_at(0,0,*stone,0);
		lcd_draw_stone(x,y,*stone,0);
		wait_ms(1000);
		count++;
		
		/* small tetris animation */
	
	}
	
	
	
	/* end tetris animation */

	
	lcd_cls();

}

