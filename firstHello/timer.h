#ifndef _TIMER_H
#define _TIMER_H

#define KEY_PIN			PINA
#define KEY_ENTER		PA7
#define KEY_ESC			PA6
#define KEY_PLUS		PA5
#define KEY_MINUS		PA4
#define KEY_ALL			((1 << KEY_PLUS) | (1 << KEY_MINUS) | (1 << KEY_ENTER) | (1 << KEY_ESC))
#define REPEAT_MASK	((1 << KEY_PLUS) | (1 << KEY_MINUS) | (1 << KEY_ENTER) | (1 << KEY_ESC))	// repeat: MODE

#define REPEAT_START	50	// after 500ms
#define REPEAT_NEXT		10	// every 100ms

#define ABO_TIMEOUT 300	// 3 sec

extern volatile uint16_t timer;
extern volatile uint16_t abo_timer;




void TIMER0_Init (void);

uint8_t get_key_press (uint8_t key_mask);
uint8_t get_key_rpt (uint8_t key_mask);
uint8_t get_key_short (uint8_t key_mask);
uint8_t get_key_long (uint8_t key_mask);
uint8_t get_key_long2 (uint8_t key_mask);
uint8_t get_key_long_rpt (uint8_t key_mask);


#endif
