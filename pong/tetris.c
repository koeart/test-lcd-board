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


extern int debug;
 
int x0min; //game area
int y0min;
int x0max; 
int y0max;




void tetris() {
	ausrichtung = rechts;
	uint8_t x; uint8_t y;
	x=10;
	y=0;
	if (debug)	{
		lcd_write_number_u(ausrichtung);
		wait_ms(5000);
	}
	lcd_cls();
	for(y=0;y<120;y++)
	{
		
		lcd_printp_at(x,y,PSTR("Tetris"),0);
		wait_ms(100);
	}
	while(x>0) {
		lcd_printp_at(x,y,PSTR("Tetris"),0);
		x--;
		wait_ms(100);
	}

	
	lcd_cls();

}

