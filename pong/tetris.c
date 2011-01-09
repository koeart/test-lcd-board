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
	if (debug)	{
		lcd_write_number_u(ausrichtung);
		wait_ms(5000);
	}
	lcd_cls();
	lcd_printp_at(0,21,PSTR("TETRIS!\r\n"),0);
	wait_ms(5000);
	lcd_cls();

}

