
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <math.h>

#include "main.h"
#include "lcd.h"
#include "usart.h"
#include "timer.h"

void wait_for_ok(void);

int main (void)
{
	// enable pull ups for the 4 keys
	PORTA |= (1<<PORTA4)|(1<<PORTA5)|(1<<PORTA6)|(1<<PORTA7);

    // disable bootloader LED
    DDRC |= (1<<DDC3);
	PORTC |= (1<<PORTC3);
            
	
	// set LED Pins to output (are low per default, so active)
	DDRC |= (1<<DDC0)|(1<<DDC1);
	DDRD |= (1<<DDD7);



	LCD_Init ();
	USART_Init ();
	TIMER0_Init ();
	sei ();

	lcd_cls ();
 

/* Koordinatensystem */

int16_t radius = 2;
int16_t xmin = 0;
int16_t xmax = 128 ;
int16_t ymin = 0;
int16_t ymax = 64;

int p1points = 10;
int cont = 1;

/* *******************************
 * Deklaration von zwei Kreisen k1 und k2
 * diese sollen wild durch die gegend bouncen
 * bei berührung -> invertierung
********************************/
int16_t k1xcord = 23;
int16_t k1ycord = 42;
int16_t k1xstep = 3;
int16_t k1ystep = 1;
int16_t k1xmaxcord;
int16_t k1xmincord;
int16_t k1ymaxcord;
int16_t k1ymincord;



int16_t r1xcord = 10;
int16_t r1ycord = 27;
int16_t r1widthx = 2;
int16_t r1widthy = 10;
int16_t r1step = 1;


int16_t r1xmaxcord;
int16_t r1xmincord;
int16_t r1ymaxcord;
int16_t r1ymincord;

r1xmaxcord = (r1xcord + r1widthx);
r1xmincord = r1xcord;


while(cont)	{
	
k1xmaxcord = (k1xcord + radius);
k1xmincord = (k1xcord - radius);
k1ymaxcord = (k1ycord + radius);
k1ymincord = (k1ycord - radius);

//STEUERUNG ***********************
	//hoch
		while (get_key_rpt (1 << KEY_MINUS)) {
			if (ymin < r1ymincord) {
			r1ycord = (r1ycord - r1step);
			//lcd_printp_at(3,0, PSTR("+\r"),0);
			 
			}
			r1ymincord = r1ycord;
			r1ymaxcord = r1ymincord + r1widthy;
			
		}
	//runter
		while (get_key_rpt (1 << KEY_PLUS)) {
			if (ymax > r1ymaxcord) {
				r1ycord = (r1ycord + r1step);
				//lcd_printp_at(3,0, PSTR("-\r"),0);
			}
			r1ymaxcord = (r1ycord + r1widthy);
			r1ymincord = r1ymaxcord - r1widthy;
			
		}
		//lcd_write_number_u_at (3,0, r1ymincord);
		//lcd_write_number_u_at (5,0, r1ymaxcord);
		lcd_frect (r1xcord, r1ycord, r1widthx, r1widthy, 1);
// STEUERUNG ENDE ***********************

	
// BALL ABFRAGE **************************	
//lcd_write_number_u_at (7,0,	r1xmaxcord);
//lcd_write_number_u_at (10,0, k1xmincord);
//lcd_write_number_u_at (13, 0, k1ycord);
		if (((r1ymaxcord > k1ycord) & (r1ymincord < k1ycord)) & (r1xmaxcord >= k1xmincord)) {
			k1xstep = -(k1xstep);
		}
		if (xmax <= k1xmaxcord) {
			k1xstep = -(k1xstep);
		}
		if (xmin >= k1xmincord) {
			p1points--;
			if (p1points == 0) {
				cont = 0;
				lcd_cls();
				lcd_printp_at (2, 3,PSTR("VERLOREN!\r\n"),0);
				wait_ms(5000);
			}
			k1xcord = 23;
			k1ycord = 42;
			k1xstep = 3;
			k1ystep = 1;
			
		}
		
		if ((k1ymaxcord >= ymax) || (k1ymincord <= ymin)) {
			k1ystep = -(k1ystep);
		}
		
		k1xcord += k1xstep;
		k1ycord += k1ystep;
		
		lcd_fcircle (k1xcord, k1ycord, radius);
		
// BALLABFRAGE ENDE *****************************

//Zeichnen		
		lcd_write_number_u_at (0,0, p1points);
		wait_ms (120);
		lcd_cls ();
		
		
		
/*	TASTENDEFINITIONEN
 * 	if (get_key_press (1 << KEY_PLUS))
		{
			lcd_printp (PSTR("PLUS\r\n"),0);
			wait_ms (1000);
		}
		if (get_key_press (1 << KEY_MINUS))
		{
			lcd_printp (PSTR("MINUS\r\n"),0);
			wait_ms (1000);
		}
		if (get_key_press (1 << KEY_ENTER))
		{
			lcd_printp (PSTR("ENTER\r\n"),0);
			wait_ms (1000);
		}
		if (get_key_press (1 << KEY_ESC))
		{
			lcd_printp (PSTR("ESC\r\n"),0);
			wait_ms (1000);
		}
*/

	}

}

		
