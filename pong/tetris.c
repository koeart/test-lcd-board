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

int xmax;
int ymax;



void tetris() {
	lcd_printp_at(4,2,PSTR("TETRIS!\r\n"),0);
	wait_ms(1000);
}

