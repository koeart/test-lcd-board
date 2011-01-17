#ifndef _MAIN_H
#define _MAIN_H

uint8_t xmin;
uint8_t xmax;
uint8_t ymin;
uint8_t ymax;
uint8_t debug;

enum displ_orientation {
	unten, rechts
	};

extern enum displ_orientation ausrichtung;

//void rotate_ko(displ_orientation normal, displ_orientation actual);



#endif
