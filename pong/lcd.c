#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "font8x6.h"
#include "main.h"
#include "lcd.h"

#define DISP_W 128
#define DISP_H 64

#define DISP_BUFFER ((DISP_H * DISP_W) / 8)



volatile uint8_t display_buffer[DISP_BUFFER];	// Display-Puffer, weil nicht zurückgelesen werden kann
volatile uint16_t display_buffer_pointer;			// Pointer auf das aktuell übertragene Byte
volatile uint8_t display_buffer_counter;			// Hilfszähler zur Selektierung der Page
volatile uint8_t display_page_counter;				// aktuelle Page-Nummer
volatile uint8_t display_mode;								// Modus für State-Machine
volatile uint8_t LCD_ORIENTATION;

// DOG: 128 x 64 with 6x8 Font => 21 x 8
// MAX7456: 30 x 16

/* display is connected to PORTBx
 * pins -> lcd.h
 * */

uint8_t lcd_xpos;
uint8_t lcd_ypos;


void send_byte (uint8_t data)	//send via SPI
{
	clr_cs ();
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	//SPSR = SPSR;
	set_cs ();
}


void lcd_cls (void)
{
	uint16_t i, j;
	
//	memset (display_buffer, 0, 1024);
	for (i = 0; i < DISP_BUFFER; i++)
		display_buffer[i] = 0x00;
	
	for (i = 0; i < 8; i++)		//i -> page adress, Write all px on a page
	{
		clr_A0 ();				//lcd: command mode
		send_byte (0xB0 + i);	//1011xxxx -> lcd: page address set
		send_byte (0x10);		//00010000 -> lcd: column address set msb(==0x00)
//		send_byte(0x04);		//00000100 gedreht plus 4 Byte
//		send_byte(0x00);		//00000000
		send_byte (LCD_ORIENTATION);	//00000000 -> lcd: column address set lsb(==0x00)

		set_A0 ();				//lcd: write mode
		for (j = 0; j < 128; j++)
			send_byte (0x00);  //write data to all 128 columns(==fill page)
	}

	lcd_xpos = 0;
	lcd_ypos = 0;
}

void wait_1ms (void)
{
	_delay_ms (1.0);
}

void wait_ms (uint16_t time)
{
	uint16_t i;
	
	for (i = 0; i < time; i++)
		wait_1ms ();
}

void/ LCD_Init (void)
{
	lcd_xpos = 0;
	lcd_ypos = 0;

	DDRB = 0xFF;

	/* SPI max. speed
	* the DOGM128 lcd controller can work at 20 MHz
	* compare to datasheet of atmel644, page 157
	*/
	
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPHA) | (1 << CPOL); //SPI Control Register
	SPSR = (1 << SPI2X);		//SPI Status Register
	
	set_cs ();
	clr_reset ();
	wait_ms (10);
	set_reset ();
	
	clr_cs ();
	clr_A0 ();			//lcd: commmand mode (a0 == 0)
	
	/* lcd: compare to initialisation example (low power mode)
	 * lcd: dogl128-6e.pdf page 6
	 */

	send_byte (0x40); 

	if (LCD_ORIENTATION == 0)
	{
		send_byte (0xA1); // $A1 normal $A0 reverse(original)
		send_byte (0xC0); // $C0 normal $C8 reverse(original)
	}
	else
	{
		send_byte (0xA0); // $A1 normal $A0 reverse(original)
		send_byte (0xC8); // $C0 normal $C8 reverse(original)
	}
	send_byte (0xA6);
	send_byte (0xA2);
	send_byte (0x2F);
	send_byte (0xF8);
	send_byte (0x00);
	send_byte (0x27);
	send_byte (0x81);
	send_byte (0x16);
	send_byte (0xAC);
	send_byte (0x00);
	send_byte (0xAF);

	lcd_cls ();
}


void set_adress (uint16_t adress, uint8_t data) //adress format: 0xppcc pp page, cc column
{
	uint8_t page;
	uint8_t column;
	
	page = adress >> 7;
	
	clr_A0 ();
	send_byte (0xB0 + page);
	
//	column = (adress & 0x7F) + 4; Wenn gedreht
//	column = (adress & 0x7F);
	column = (adress & 0x7F) + LCD_ORIENTATION;

	send_byte (0x10 + (column >> 4)); 	//send high part
	send_byte (column & 0x0F);			//send low part
	
	set_A0 ();
	send_byte (data);
}


void scroll (void)
{
	uint16_t adress;
	
	for (adress = 0; adress < 896; adress++)
	{
		display_buffer[adress] = display_buffer[adress + 128];
		set_adress (adress, display_buffer[adress]);
	}
	for (adress = 896; adress < 1024; adress++)
	{
		display_buffer[adress] = 0;
		set_adress (adress, 0);
	}
}


//
// x,y = character-Pos. !
//
// mode: 0=Overwrite, 1 = OR, 2 = XOR, 3 = AND, 4 = Delete
void lcd_putc (uint8_t x, uint8_t y, uint8_t c, uint8_t mode)
{
	uint8_t ch;
	uint8_t i;
	uint16_t adress;

	switch (c)
	{	// ISO 8859-1
		case 0xc4:	// Ä
			c = 0x00;
			break;
		case 0xe4:	// ä
			c = 0x01;
			break;
		case 0xd6:	// Ö
			c = 0x02;
			break;
		case 0xf6:	// ö
			c = 0x03;
			break;
		case 0xdc:	// Ü
			c = 0x04;
			break;
		case 0xfc:	// ü
			c = 0x05;
			break;
		case 0xdf:	// ß
			//c = 0x06;
			c = 0x1e; // ° (used by Jeti)
			break;
	}

	c &= 0x7f;
	
	adress = y * 128 + x * 6;
	adress &= 0x3FF;
		
	for (i = 0; i < 6; i++)
	{
		ch = pgm_read_byte (&font8x6[0][0] + i + c * 6);

		switch (mode)
		{
			case 0:
				display_buffer[adress+i] = ch;
				break;
			case 1:
				display_buffer[adress+i] |= ch;
				break;
			case 2:
				display_buffer[adress+i] ^= ch;
				break;
			case 3:
				display_buffer[adress+i] &= ch;
				break;
			case 4:
				display_buffer[adress+i] &= ~ch;
				break;
		}
		
		set_adress (adress + i, display_buffer[adress + i]);
	}
}


void new_line (void)
{
	lcd_ypos++;
	
	if (lcd_ypos > 7)
	{
		scroll ();
		lcd_ypos = 7;
	}
}



void lcd_printpns (const char *text, uint8_t mode)
{
	while (pgm_read_byte(text))
	{
		switch (pgm_read_byte(text))
		{
			case 0x0D:
				lcd_xpos = 0;
				break;
			case 0x0A:
				new_line();
				break;
			default:
				lcd_putc (lcd_xpos, lcd_ypos, pgm_read_byte(text), mode);
				
				lcd_xpos++;
				if (lcd_xpos > 20)
				{
					lcd_xpos = 0;
//					new_line ();
				}
				break;
		}
		text++;
	}
}


void lcd_printpns_at (uint8_t x, uint8_t y, const char *text, uint8_t mode)
{
	lcd_xpos = x;
	lcd_ypos = y;
	lcd_printpns (text, mode);
}


void lcd_printp (const char *text, uint8_t mode)
{
	while (pgm_read_byte(text))
	{
		switch (pgm_read_byte(text))
		{
			case 0x0D:
				lcd_xpos = 0;
				break;
			case 0x0A:
				new_line();
				break;
			default:
				lcd_putc (lcd_xpos, lcd_ypos, pgm_read_byte(text), mode);
				
				lcd_xpos++;
				if (lcd_xpos > 20)
				{
					lcd_xpos = 0;
					new_line ();
				}
				break;
		}
		text++;
	}
}


void lcd_printp_at (uint8_t x, uint8_t y, const char *text, uint8_t mode)
{
	lcd_xpos = x;
	lcd_ypos = y;
	lcd_printp (text, mode);
}


void lcd_print (uint8_t *text, uint8_t mode)
{
	while (*text)
	{
		switch (*text)
		{
			case 0x0D:
				lcd_xpos = 0;
				break;
			case 0x0A:
				new_line();
				break;
			default:
				lcd_putc (lcd_xpos, lcd_ypos, *text, mode);
				
				lcd_xpos++;
				if (lcd_xpos > 20)
				{
					lcd_xpos = 0;
					new_line ();
				}
				break;
		}
		text++;
	}
}

void lcd_print_at (uint8_t x, uint8_t y, uint8_t *text, uint8_t mode)
{
	lcd_xpos = x;
	lcd_ypos = y;
	lcd_print (text, mode);
}


void print_display (uint8_t *text)
{
	while (*text)
	{
		lcd_putc (lcd_xpos, lcd_ypos, *text, 0);
				
		lcd_xpos++;
		if (lcd_xpos >= 20)
		{
			lcd_xpos = 0;
			new_line ();
		}
		text++;
	}
}

void print_display_at (uint8_t x, uint8_t y, uint8_t *text)
{
	lcd_xpos = x;
	lcd_ypos = y;
	print_display (text);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Plot (set one Pixel)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// mode:
// 0=Clear, 1=Set, 2=XOR
void lcd_plot (uint8_t xpos, uint8_t ypos, uint8_t mode)
{
	uint16_t adress;
	uint8_t mask;
	if(ausrichtung == unten) {
		if ((xpos < DISP_W) && (ypos < DISP_H))
		{
		adress = (ypos / 8) * DISP_W + xpos;		// adress = 0/8 * 128 + 0   = 0
		mask = 1 << (ypos & 0x07);					// mask = 1<<0 = 1
		
		adress &= DISP_BUFFER - 1;
		
		switch (mode)
		{
			case 0:
				display_buffer[adress] &= ~mask;
				break;
			case 1:
				display_buffer[adress] |= mask;
				break;
			case 2:
				display_buffer[adress] ^= mask;
				break;
		}
		
		set_adress (adress, display_buffer[adress]);
		}
	}
	if(ausrichtung == rechts) {
		if ((xpos < DISP_W) && (ypos < DISP_H))
		{
		adress = (ypos / 8) * DISP_W + xpos;		// adress = 0/8 * 128 + 0   = 0
		mask = 1 << (ypos & 0x07);					// mask = 1<<0 = 1
		
		adress &= DISP_BUFFER - 1;
		
		switch (mode)
		{
			case 0:
				display_buffer[adress] &= ~mask;
				break;
			case 1:
				display_buffer[adress] |= mask;
				break;
			case 2:
				display_buffer[adress] ^= mask;
				break;
		}
		
		set_adress (adress, display_buffer[adress]);
		}
	}
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Line (draws a line from x1,y1 to x2,y2
// + Based on Bresenham line-Algorithm
// + found in the internet, modified by thkais 2007
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void lcd_line (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, uint8_t mode)
{
	int x, y, count, xs, ys, xm, ym;

	x = (int) x1;
	y = (int) y1;
	xs = (int) x2 - (int) x1;
	ys = (int) y2 - (int) y1;
	if (xs < 0) 
		xm = -1;
	else
		if (xs > 0)
			xm = 1;
		else
			xm = 0;
	if (ys < 0)
		ym = -1;
	else
		if (ys > 0)
			ym = 1;
		else
			ym = 0;
	if (xs < 0)
		xs = -xs;

	if (ys < 0)
		ys = -ys;

	lcd_plot ((unsigned char) x, (unsigned char) y, mode);

	if (xs > ys) // Flat Line <45 degrees
	{
		count = -(xs / 2);
		while (x != x2)
		{
			count = count + ys;
			x = x + xm;
			if (count > 0)
			{
				y = y + ym;
				count = count - xs;
			}
			lcd_plot ((unsigned char) x, (unsigned char) y, mode);
		}
	}
	else // Line >=45 degrees
	{
		count =- (ys / 2);
		while (y != y2)
		{
			count = count + xs;
			y = y + ym;
			if (count > 0)
			{
				x = x + xm;
				count = count - ys;
			}
			lcd_plot ((unsigned char) x, (unsigned char) y, mode);
		}
	}
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Filled rectangle
// + x1, y1 = upper left corner
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void lcd_frect (uint8_t x1, uint8_t y1, uint8_t widthx, uint8_t widthy, uint8_t mode)
{
	uint16_t x2, y2;
	uint16_t i;

	if (x1 >= DISP_W)
		x1 = DISP_W - 1;

	if (y1 >= DISP_H)
		y1 = DISP_H - 1;

	x2 = x1 + widthx;
	y2 = y1 + widthy;
	
	if (x2 > DISP_W)
		x2 = DISP_W;

	if (y2 > DISP_H)
		y2 = DISP_H;
		
	for (i = y1; i <= y2; i++)
	{
		lcd_line (x1, i, x2, i, mode);
	}
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + outline of rectangle
// + x1, y1 = upper left corner
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void lcd_rect (uint8_t x1, uint8_t y1, uint8_t widthx, uint8_t widthy, uint8_t mode)
{
	uint16_t x2, y2;

	if (x1 >= DISP_W)
		x1 = DISP_W - 1;
	if (y1 >= DISP_H)
		y1 = DISP_H - 1;
	x2 = x1 + widthx;
	y2 = y1 + widthy;
	
	if (x2 > DISP_W)
		x2 = DISP_W;
	
	if (y2 > DISP_H)
		y2 = DISP_H;

	lcd_line (x1, y1, x2, y1, mode);
	lcd_line (x2, y1, x2, y2, mode);
	lcd_line (x2, y2, x1, y2, mode);
	lcd_line (x1, y2, x1, y1, mode);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + outline of a circle
// + Based on Bresenham-algorithm found in wikipedia
// + modified by thkais (2007)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void lcd_circle (int16_t x0, int16_t y0, int16_t radius, uint8_t mode)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	int16_t x = 0;
	int16_t y = radius;

	lcd_plot (x0, y0 + radius, mode);
	lcd_plot (x0, y0 - radius, mode);
	lcd_plot (x0 + radius, y0, mode);
	lcd_plot (x0 - radius, y0, mode);

	while (x < y) 
	{
		if (f >= 0) 
		{
			y --;
			ddF_y += 2;
			f += ddF_y;
		}
		x ++;
		ddF_x += 2;
		f += ddF_x + 1;

		lcd_plot (x0 + x, y0 + y, mode);
		lcd_plot (x0 - x, y0 + y, mode);
		 
		lcd_plot (x0 + x, y0 - y, mode);
		lcd_plot (x0 - x, y0 - y, mode);
		 
		lcd_plot (x0 + y, y0 + x, mode);
		lcd_plot (x0 - y, y0 + x, mode);
		 
		lcd_plot (x0 + y, y0 - x, mode);
		lcd_plot (x0 - y, y0 - x, mode);
	}
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + filled Circle
// + modified circle-algorithm thkais (2007)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void lcd_fcircle (int16_t x0, int16_t y0, int16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	int16_t x = 0;
	int16_t y = radius;
 
	lcd_line (x0, y0 + radius, x0, y0 - radius, 1);
   
	lcd_line (x0 + radius, y0, x0 - radius, y0, 1);
    
	while (x < y) 
	{
		if (f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
 
		lcd_line (x0 + x, y0 + y, x0 - x, y0 + y, 1);
		lcd_line (x0 + x, y0 - y, x0 - x, y0 - y, 1);
		lcd_line (x0 + y, y0 + x, x0 - y, y0 + x, 1);
		lcd_line (x0 + y, y0 - x, x0 - y, y0 - x, 1);
	}
}

//*****************************************************************************
// 
void lcd_circ_line (uint8_t x, uint8_t y, uint8_t r, uint16_t deg, uint8_t mode)
{
	uint8_t xc, yc;
	double deg_rad;

	deg_rad = (deg * M_PI) / 180.0;

	yc = y - (uint8_t) round (cos (deg_rad) * (double) r);
	xc = x + (uint8_t) round (sin (deg_rad) * (double) r);
	lcd_line (x, y, xc, yc, mode);
}

//*****************************************************************************
// 
void lcd_ellipse_line (uint8_t x, uint8_t y, uint8_t rx, uint8_t ry, uint16_t deg, uint8_t mode)
{
	uint8_t xc, yc;
	double deg_rad;
	
	deg_rad = (deg * M_PI) / 180.0;
	
	yc = y - (uint8_t) round (cos (deg_rad) * (double) ry);
	xc = x + (uint8_t) round (sin (deg_rad) * (double) rx);
	lcd_line (x, y, xc, yc, mode);
}

//*****************************************************************************
// 
void lcd_ellipse (int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint8_t mode)
{
	const int16_t rx2 = rx * rx;
	const int16_t ry2 = ry * ry;
	int16_t F = round (ry2 - rx2 * ry + 0.25 * rx2);
	int16_t ddF_x = 0;
	int16_t ddF_y = 2 * rx2 * ry;
	int16_t x = 0;
	int16_t y = ry;
	
	lcd_plot (x0, y0 + ry, mode);
	lcd_plot (x0, y0 - ry, mode);
	lcd_plot (x0 + rx, y0, mode);
	lcd_plot (x0 - rx, y0, mode);
	// while ( 2*ry2*x < 2*rx2*y ) {  we can use ddF_x and ddF_y
	while (ddF_x < ddF_y)
	{
		if(F >= 0)
		{
			y     -= 1;        // south
			ddF_y -= 2 * rx2;
			F     -= ddF_y;
		}
		x     += 1;          // east
		ddF_x += 2 * ry2;
		F     += ddF_x + ry2;
		lcd_plot (x0 + x, y0 + y, mode);
		lcd_plot (x0 + x, y0 - y, mode);
		lcd_plot (x0 - x, y0 + y, mode);
		lcd_plot (x0 - x, y0 - y, mode);
	}
	F = round (ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
	while(y > 0)
	{
		if(F <= 0)
		{
			x     += 1;        // east
			ddF_x += 2 * ry2;
			F     += ddF_x;
		}
		y     -=1;           // south
		ddF_y -= 2 * rx2;
		F     += rx2 - ddF_y;
		lcd_plot (x0 + x, y0 + y, mode);
		lcd_plot (x0 + x, y0 - y, mode);
		lcd_plot (x0 - x, y0 + y, mode);
		lcd_plot (x0 - x, y0 - y, mode);
	}
}

//*****************************************************************************
// 
void lcd_ecircle (int16_t x0, int16_t y0, int16_t radius, uint8_t mode)
{
	lcd_ellipse (x0, y0, radius + 3, radius, mode);
}

//*****************************************************************************
// 
void lcd_ecirc_line (uint8_t x, uint8_t y, uint8_t r, uint16_t deg, uint8_t mode)
{
	lcd_ellipse_line(x, y, r + 3, r, deg, mode);
}

//*****************************************************************************
// 
void lcd_view_font (uint8_t page)
{
	int x;
	int y;

	lcd_cls ();
	lcd_printp (PSTR("  0123456789ABCDEF\r\n"), 0);
//	lcd_printpns_at (0, 7, PSTR(" \x16    \x17     Exit"), 0);
	lcd_printpns_at (0, 7, PSTR(" \x1a    \x1b     Exit"), 0);

	lcd_ypos = 2;
	for (y = page * 4 ; y < (page * 4 + 4); y++)
	{
		if (y < 10)
		{
			lcd_putc (0, lcd_ypos, '0' + y, 0);
		}
		else
		{
			lcd_putc (0, lcd_ypos, 'A' + y - 10, 0);
		}
		lcd_xpos = 2;
		for (x = 0; x < 16; x++)
		{
			lcd_putc (lcd_xpos, lcd_ypos, y * 16 + x, 0);
			lcd_xpos++;
		}
		lcd_ypos++;
	}
}

uint8_t hdigit (uint8_t d)
{
	if (d < 10)
	{
		return '0' + d;
	}
	else
	{
		return 'A' + d - 10;
	}
}

void lcd_print_hex_at (uint8_t x, uint8_t y, uint8_t h, uint8_t mode)
{
	lcd_xpos = x;
	lcd_ypos = y;
	
	lcd_putc (lcd_xpos++, lcd_ypos, hdigit (h >> 4), mode);
	lcd_putc (lcd_xpos, lcd_ypos, hdigit (h & 0x0f), mode);
}

void lcd_write_number_u (uint8_t number)
{
	uint8_t num = 100;
	uint8_t started = 0;
	
	while (num > 0)
	{
		uint8_t b = number / num;
		if (b > 0 || started || num == 1)
		{
			lcd_putc (lcd_xpos++, lcd_ypos, '0' + b, 0);
			started = 1;
		}
		number -= b * num;
		
		num /= 10;
	}
}

void lcd_write_number_u_at (uint8_t x, uint8_t y, uint8_t number)
{
	lcd_xpos = x;
	lcd_ypos = y;
	lcd_write_number_u (number);
}


/**
 * Write only some digits of a unsigned <number> at <x>/<y> to MAX7456 display memory
 * <num> represents the largest multiple of 10 that will still be displayable as
 * the first digit, so num = 10 will be 0-99 and so on
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_u (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad)
{
	char s[7];

	utoa(number, s, 10 );

	uint8_t len = strlen(s);

	if (length < len)
	{
		for (uint8_t i = 0; i < length; i++)
		{
			lcd_putc (x++, y, '*', 0);
		}
		return;
	}

	for (uint8_t i = 0; i < length - len; i++)
	{
		if (pad)
		{
			lcd_putc (x++, y, '0', 0);
		}
		else
		{
			lcd_putc (x++, y, ' ', 0);
		}
	}
	lcd_print_at(x, y, (uint8_t*)s, 0);
}

/**
 * Write only some digits of a signed <number> at <x>/<y> to MAX7456 display memory
 * <num> represents the largest multiple of 10 that will still be displayable as
 * the first digit, so num = 10 will be 0-99 and so on
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_s (uint8_t x, uint8_t y, int16_t number, int16_t length, uint8_t pad)
{
	char s[7];
	
	itoa(number, s, 10 );

	uint8_t len = strlen(s);

	if (length < len)
	{
		for (uint8_t i = 0; i < length; i++)
		{
			lcd_putc (x++, y, '*', 0);
		}
		return;
	}

	for (uint8_t i = 0; i < length - len; i++)
	{
		if (pad)
		{
			lcd_putc (x++, y, '0', 0);
		}
		else
		{
			lcd_putc (x++, y, ' ', 0);
		}
	}
	lcd_print_at(x, y, (uint8_t*)s, 0);
}

/**
 * Write only some digits of a unsigned <number> at <x>/<y> to MAX7456 display memory
 * as /10th of the value
 * <num> represents the largest multiple of 10 that will still be displayable as
 * the first digit, so num = 10 will be 0-99 and so on
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_u_10th (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad)
{
	char s[7];
	
	itoa(number, s, 10 );

	uint8_t len = strlen(s);

	if (length < len)
	{
		for (uint8_t i = 0; i < length; i++)
		{
			lcd_putc (x++, y, '*', 0);
		}
		return;
	}

	for (uint8_t i = 0; i < length - len; i++)
	{
		if (pad)
		{
			lcd_putc (x++, y, '0', 0);
		}
		else
		{
			lcd_putc (x++, y, ' ', 0);
		}
	}

	char rest = s[len - 1];

	s[len - 1] = 0;

	if (len == 1)
	{
		lcd_putc (x-1, y, '0', 0);
	}
	else if (len == 2 && s[0] == '-')
	{
		lcd_putc (x-1, y, '-', 0);
		lcd_putc (x, y, '0', 0);
	}
	else
	{
		lcd_print_at(x, y, (uint8_t*)s, 0);
	}
	x += len - 1;
	lcd_putc (x++, y, '.', 0);
	lcd_putc (x++, y, rest, 0);
}

void write_ndigit_number_u_100th (uint8_t x, uint8_t y, uint16_t number, int16_t length, uint8_t pad)
{
	uint8_t num = 100;
	
	while (num > 0)
	{
		uint8_t b = number / num;

		if ((num / 10) == 1)
		{
			lcd_putc (x++, y, '.', 0);
		}
		lcd_putc (x++, y, '0' + b, 0);
		number -= b * num;
		
		num /= 10;
	}
}

/**
 * Write only some digits of a signed <number> at <x>/<y> to MAX7456 display memory
 * as /10th of the value
 * <num> represents the largest multiple of 10 that will still be displayable as
 * the first digit, so num = 10 will be 0-99 and so on
 * <pad> = 1 will cause blank spaced to be filled up with zeros e.g. 007 instead of   7
 */
void write_ndigit_number_s_10th (uint8_t x, uint8_t y, int16_t number, int16_t length, uint8_t pad)
{
	char s[7];

	itoa (number, s, 10 );

	uint8_t len = strlen(s);

	if (length < len)
	{
		for (uint8_t i = 0; i < length; i++)
		{
			lcd_putc (x++, y, '*', 0);
		}
		return;
	}

	for (uint8_t i = 0; i < length - len; i++)
	{
		if (pad)
		{
			lcd_putc (x++, y, '0', 0);
		}
		else
		{
			lcd_putc (x++, y, ' ', 0);
		}
	}

	char rest = s[len - 1];

	s[len - 1] = 0;

	if (len == 1)
	{
		lcd_putc (x-1, y, '0', 0);
	}
	else if (len == 2 && s[0] == '-')
	{
		lcd_putc (x-1, y, '-', 0);
		lcd_putc (x, y, '0', 0);
	}
	else
	{
		lcd_print_at(x, y, (uint8_t*)s, 0);
	}
	x += len - 1;
	lcd_putc (x++, y, '.', 0);
	lcd_putc (x++, y, rest, 0);
}

