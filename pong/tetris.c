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



 
int x0min; //game area
int y0min;
int x0max; 
int y0max;

void layout(void) {
	lcd_line (0,0,64,0,0);
	}


void tetris() {
	ausrichtung = rechts;
	lcd_write_number_u(ausrichtung);
	wait_ms(5000);
	lcd_printp_at(4,2,PSTR("TETRIS!\r\n"),0);
	wait_ms(1000);
}

