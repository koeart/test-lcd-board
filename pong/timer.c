#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "timer.h"

volatile uint16_t timer;
volatile uint16_t abo_timer;

#define FRAME_LENGTH 20
uint16_t icrval = (F_CPU / 64) * FRAME_LENGTH / 1000 ;
uint16_t minocr_a = (F_CPU / 64) * 1            / 1000;// 312
uint16_t maxocr_a = (F_CPU / 64) * 2            / 1000;// 624
uint16_t minocr_b = (F_CPU / 64) * 1           / 1000;// 312
uint16_t maxocr_b = (F_CPU / 64) * 2            / 1000;// 624


uint8_t key_state = 0;	// debounced and inverted key state:
												// bit = 1: key pressed
uint8_t key_press = 0;	// key press detect
uint8_t key_rpt;				// key long press and repeat


//*****************************************************************************
// 


#if defined (__AVR_ATmega32__)
ISR(TIMER0_COMP_vect)						// Timer-Interrupt (100 Hz)
#else
ISR(TIMER0_COMPA_vect)					// Timer-Interrupt (100 Hz)
#endif
{
  static uint8_t ct0 = 0;
	static uint8_t ct1 = 0;
	static uint8_t rpt = 0;
	uint8_t i;
	
	// Key handling by Peter Dannegger
	// see www.mikrocontroller.net
	i = key_state ^ ~KEY_PIN;	// key changed ?
	ct0 = ~(ct0 & i);						// reset or count ct0
	ct1 = ct0 ^ (ct1 & i);			// reset or count ct1
	i &= (ct0 & ct1);						// count until roll over ?
	key_state ^= i;							// then toggle debounced state
	key_press |= (key_state & i);	// 0->1: key press detect
	
	if ((key_state & REPEAT_MASK) == 0)	// check repeat function
	{
		rpt = REPEAT_START;	// start delay
	}
	if (--rpt == 0)
	{
		rpt = REPEAT_NEXT;	// repeat delay
		key_rpt |= (key_state & REPEAT_MASK);
	}

	
	if (timer > 0)
	{
		timer --;
	}

	if (abo_timer > 0)
	{
		abo_timer --;
	}

}

    


//*****************************************************************************
// 
void TIMER0_Init (void)
{
	timer = 0;
	
#if defined (__AVR_ATmega32__)
	TCCR0 = (1 << CS02) | (1 << CS00) | (1 << WGM01);		// Prescaler 1024
	OCR0 = (F_CPU / (100L * 1024L)) ;

	TIMSK |= (1 << OCIE0);	// enable interrupt for OCR
#else
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS02) | (1 << CS00);
	OCR0A = (F_CPU / (100L * 1024L)) ;

	TIMSK0 |= (1 << OCIE0A);	// enable interrupt for OCR
#endif
}


//*****************************************************************************
// 
uint8_t get_key_press (uint8_t key_mask)
{
	uint8_t sreg = SREG;
	
	// disable all interrupts
	cli();
	
  key_mask &= key_press;	// read key(s)
  key_press ^= key_mask;	// clear key(s)
	
	SREG = sreg;	// restore status register
	
  return key_mask;
}


//*****************************************************************************
// 
uint8_t get_key_rpt (uint8_t key_mask)
{
	uint8_t sreg = SREG;

	// disable all interrupts
	cli();
	
  key_mask &= key_rpt;	// read key(s)
  key_rpt ^= key_mask;	// clear key(s)
	
	SREG = sreg;	// restore status register
	
  return key_mask;
}


//*****************************************************************************
// 
uint8_t get_key_short (uint8_t key_mask)
{
	uint8_t ret;
	uint8_t sreg = SREG;
	
	// disable all interrupts
	cli();
	
  ret = get_key_press (~key_state & key_mask);
	
	SREG = sreg;	// restore status register
	
  return ret;
}


//*****************************************************************************
// 
uint8_t get_key_long (uint8_t key_mask)
{
  return get_key_press (get_key_rpt (key_mask));
}


//*****************************************************************************
// 
uint8_t get_key_long2 (uint8_t key_mask)
{
  return get_key_press (get_key_rpt (key_press^key_mask));
}


//*****************************************************************************
// 
uint8_t get_key_long_rpt (uint8_t key_mask)
{
  return get_key_rpt (~key_press^key_mask);
}
