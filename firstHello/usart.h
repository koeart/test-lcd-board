#ifndef _USART_H
#define _USART_H

//*****************************************************************************
// 
#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif


// must be at least 4('#'+Addr+'CmdID'+'\r')+ (80 * 4)/3 = 111 bytes
#define TXD_BUFFER_LEN  60
#define RXD_BUFFER_LEN  180

// Baud rate of the USART
#define USART_BAUD 115200	
//#define USART_BAUD 125000	

//*****************************************************************************
// 
extern uint8_t buffer[30];

extern volatile uint8_t txd_buffer[TXD_BUFFER_LEN];
extern volatile uint8_t txd_complete;
extern volatile uint8_t rxd_buffer[RXD_BUFFER_LEN];
extern volatile uint8_t rxd_buffer_locked;
extern volatile uint8_t ReceivedBytes;
extern volatile uint8_t *pRxData;
extern volatile uint8_t RxDataLen;

extern volatile uint16_t stat_crc_error;
extern volatile uint16_t stat_overflow_error;

extern volatile uint8_t rxFlag;
extern volatile uint8_t rx_byte;

//*****************************************************************************
// 
void USART_Init (void);

void USART_DisableTXD (void);
void USART_EnableTXD (void);

void USART_putc (char c);
void USART_puts (char *s);
void USART_puts_p (const char *s);


uint8_t uart_getc_nb(uint8_t*);

//*****************************************************************************
//Anpassen der seriellen Schnittstellen Register
#define USART_RXC_vect USART0_RX_vect
//-----------------------
#define UCSRA		UCSR0A
#define UCSRB		UCSR0B
#define UCSRC		UCSR0C
#define UDR			UDR0
#define UBRRL		UBRR0L
#define UBRRH		UBRR0H

// UCSRA
#define	RXC			RXC0
#define TXC			TXC0
#define UDRE		UDRE0
#define FE			FE0
#define UPE			UPE0
#define U2X			U2X0
#define MPCM		MPCM0

// UCSRB
#define RXCIE		RXCIE0
#define TXCIE		TXCIE0
#define UDRIE		UDRIE0
#define TXEN		TXEN0
#define RXEN		RXEN0
#define UCSZ2		UCSZ02
#define RXB8		RXB80
#define TXB8		TXB80

// UCSRC
#define UMSEL1	UMSEL01
#define UMSEL0	UMSEL00
#define UPM1		UPM01
#define UPM0		UPM00
#define USBS		USBS0
#define UCSZ1		UCSZ01
#define UCSZ0		UCSZ00
#define UCPOL		UCPOL0
//-----------------------


#endif

