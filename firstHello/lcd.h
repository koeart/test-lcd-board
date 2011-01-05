#ifndef _LCD_H
#define _LCD_H


#define set_cs()    (PORTB |=  (1 << PB4))
#define clr_cs()    (PORTB &= ~(1 << PB4))
#define set_A0()    (PORTB |=  (1 << PB3))          // Data
#define clr_A0()    (PORTB &= ~(1 << PB3))          // Command
#define set_reset() (PORTB |=  (1 << PB2))
#define clr_reset() (PORTB &= ~(1 << PB2))

         
//*****************************************************************************
// 
extern volatile uint8_t LCD_ORIENTATION;

extern uint8_t lcd_xpos;
extern uint8_t lcd_ypos;

//*****************************************************************************
// 
void LCD_Init (void);
void new_line(void);
void lcd_putc (uint8_t x, uint8_t y, uint8_t c, uint8_t mode);
//void send_byte (uint8_t data);
void lcd_print (uint8_t *text, uint8_t mode);
void lcd_print_at (uint8_t x, uint8_t y, uint8_t *text, uint8_t mode);
void lcd_printp (const char *text, uint8_t mode);
void lcd_printp_at (uint8_t x, uint8_t y, const char *text, uint8_t mode);
void lcd_printpns (const char *text, uint8_t mode);
void lcd_printpns_at (uint8_t x, uint8_t y, const char *text, uint8_t mode);
void lcd_cls (void);

void print_display (uint8_t *text);
void print_display_at (uint8_t x, uint8_t y, uint8_t *text);


// Jeti
void lcd_putc_jeti (uint8_t x, uint8_t y, uint8_t c, uint8_t mode);
void lcd_printpj (const char *text, uint8_t mode);
void lcd_printpj_at (uint8_t x, uint8_t y, const char *text, uint8_t mode);

void lcd_plot (uint8_t x, uint8_t y, uint8_t mode);
void lcd_line (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, uint8_t mode);
void lcd_rect (uint8_t x1, uint8_t y1, uint8_t widthx, uint8_t widthy, uint8_t mode);
void lcd_frect (uint8_t x1, uint8_t y1, uint8_t widthx, uint8_t widthy, uint8_t mode);
void lcd_circle (int16_t x0, int16_t y0, int16_t radius, uint8_t mode);
void lcd_fcircle (int16_t x0, int16_t y0, int16_t radius);
void lcd_circ_line (uint8_t x, uint8_t y, uint8_t r, uint16_t deg, uint8_t mode);

void lcd_ellipse (int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint8_t mode);
void lcd_ellipse_line (uint8_t x, uint8_t y, uint8_t rx, uint8_t ry, uint16_t deg, uint8_t mode);

void lcd_ecircle (int16_t x0, int16_t y0, int16_t radius, uint8_t mode);
void lcd_ecirc_line (uint8_t x, uint8_t y, uint8_t r, uint16_t deg, uint8_t mode);

void lcd_view_font (uint8_t page);
void lcd_print_hex_at (uint8_t x, uint8_t y, uint8_t h, uint8_t mode);

void lcd_write_number_u (uint8_t number);
void lcd_write_number_u_at (uint8_t x, uint8_t y, uint8_t number);

/**
 * Write only some digits of a unsigned <number> at <x>/<y>
 * <length> represents the length to rightbound the number
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_u (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad);

/**
 * Write only some digits of a signed <number> at <x>/<y>
 * <length> represents the length to rightbound the number
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_s (uint8_t x, uint8_t y, int16_t number, int16_t length, uint8_t pad);

/**
 * Write only some digits of a unsigned <number> at <x>/<y> as /10th of the value
 * <length> represents the length to rightbound the number
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 00.7 instead of   .7
 */
void write_ndigit_number_u_10th (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad);

/**
 * Write only some digits of a unsigned <number> at <x>/<y> as /100th of the value
 * <length> represents the length to rightbound the number
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 00.7 instead of   .7
 */
void write_ndigit_number_u_100th (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad);

/**
 * Write only some digits of a signed <number> at <x>/<y> as /10th of the value
 * <length> represents the length to rightbound the number
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 00.7 instead of   .7
 */
void write_ndigit_number_s_10th (uint8_t x, uint8_t y, int16_t number, int16_t length, uint8_t pad);


#endif
