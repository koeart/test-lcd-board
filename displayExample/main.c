
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

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
	lcd_printp (PSTR("hello world!\r\n"), 0);
	

	while(1)
	{
	}

}
