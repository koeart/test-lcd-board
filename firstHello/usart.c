#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdarg.h>

#include "main.h"
#include "usart.h"
#include "lcd.h"

uint8_t buffer[30];

volatile uint8_t txd_buffer[TXD_BUFFER_LEN];
volatile uint8_t txd_complete = TRUE;
volatile uint8_t rxd_buffer[RXD_BUFFER_LEN];
volatile uint8_t rxd_buffer_locked = FALSE;
volatile uint8_t ReceivedBytes = 0;
volatile uint8_t *pRxData = 0;
volatile uint8_t RxDataLen = 0;

volatile uint16_t stat_crc_error = 0;
volatile uint16_t stat_overflow_error = 0;

volatile uint8_t rx_byte;
volatile uint8_t rxFlag = 0;


#define UART_RXBUFSIZE 64

volatile static uint8_t rxbuf[UART_RXBUFSIZE];
volatile static uint8_t *volatile rxhead, *volatile rxtail;



#ifdef USART_INT
//*****************************************************************************
// USART0 transmitter ISR
ISR (USART_TXC_vect)
{
	static uint16_t ptr_txd_buffer = 0;
	uint8_t tmp_tx;

	if(!txd_complete) // transmission not completed
	{
		ptr_txd_buffer++;                    // [0] was already sent
		tmp_tx = txd_buffer[ptr_txd_buffer];
		// if terminating character or end of txd buffer was reached
		if((tmp_tx == '\r') || (ptr_txd_buffer == TXD_BUFFER_LEN))
		{
			ptr_txd_buffer = 0;		// reset txd pointer
			txd_complete = TRUE;	// stop transmission
		}
		UDR = tmp_tx; // send current byte will trigger this ISR again
	}
	// transmission completed
	else ptr_txd_buffer = 0;
}
#endif

//*****************************************************************************
// 


uint8_t uart_getc_nb(uint8_t *c)
{
	if (rxhead==rxtail) return 0;
	*c = *rxtail;
	if (++rxtail == (rxbuf + UART_RXBUFSIZE)) rxtail = rxbuf;
	return 1;
}


ISR (USART_RXC_vect)
{
	uint8_t c;



                                                                                                        	
		int diff;
		c=UDR;
		diff = rxhead - rxtail;
		if (diff < 0) diff += UART_RXBUFSIZE;
		if (diff < UART_RXBUFSIZE -1) 
		{
			*rxhead = c;
			++rxhead;
			if (rxhead == (rxbuf + UART_RXBUFSIZE)) rxhead = rxbuf;
		};
		
		
		
		
}

//*****************************************************************************
// 
void USART_Init (void)
{
	// set clock divider
	#undef BAUD
	#define BAUD USART_BAUD
	#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
#if USE_2X
	UCSRA |= (1 << U2X);	// enable double speed operation
#else
	UCSRA &= ~(1 << U2X);	// disable double speed operation
#endif
	
	// set 8N1
#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega32__)
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
#else
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
#endif
	UCSRB &= ~(1 << UCSZ2);

	// flush receive buffer
	while ( UCSRA & (1 << RXC) ) UDR;

	UCSRB |= (1 << RXEN) | (1 << TXEN);
#ifdef USART_INT
	UCSRB |= (1 << RXCIE) | (1 << TXCIE);
#else
	UCSRB |= (1 << RXCIE);
#endif

	rxhead = rxtail = rxbuf;

}

//*****************************************************************************
// disable the txd pin of usart
void USART_DisableTXD (void)
{
#ifdef USART_INT
	UCSRB &= ~(1 << TXCIE);		// disable TX-Interrupt
#endif
	UCSRB &= ~(1 << TXEN);		// disable TX in USART
	DDRB  &= ~(1 << DDB3);		// set TXD pin as input
	PORTB &= ~(1 << PORTB3);	// disable pullup on TXD pin
}

//*****************************************************************************
// enable the txd pin of usart
void USART_EnableTXD (void)
{
	DDRB  |=  (1 << DDB3);		// set TXD pin as output
	PORTB &= ~(1 << PORTB3);	// disable pullup on TXD pin
	UCSRB |=  (1 << TXEN);		// enable TX in USART
#ifdef USART_INT
	UCSRB |=  (1 << TXCIE);		// enable TX-Interrupt
#endif
}


//*****************************************************************************
// 
void USART_putc (char c)
{
#ifdef USART_INT
#else
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
#endif
}

//*****************************************************************************
// 
void USART_puts (char *s)
{
#ifdef USART_INT
#else
	while (*s)
	{
		USART_putc (*s);
		s++;
	}
#endif
}

//*****************************************************************************
// 
void USART_puts_p (const char *s)
{
#ifdef USART_INT
#else
	while (pgm_read_byte(s))
	{
		USART_putc (pgm_read_byte(s));
		s++;
	}
#endif
}

