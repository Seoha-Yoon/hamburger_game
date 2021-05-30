//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_hexadecimal[10][MAX_DOT] = {
	{0x3C, 0x7E, 0x7E, 0x7E, 0x3C}, // Br
	{0x18, 0x24, 0x24, 0x24, 0x18}, // Let
	{0x3C, 0x7E, 0x66, 0x7E, 0x3C}, // Tmt
	{0x08, 0x1C, 0x3E, 0x1C, 0x08}, // Cs
	{0x18, 0x3C, 0x3C, 0x3C, 0x18}, // Pt
	{0x40, 0x40, 0x40, 0x40, 0x40}, // Br_b
	{0x40, 0x40, 0x40, 0x40, 0x40}, // Let_b
	{0x40, 0x40, 0x40, 0x40, 0x40}, // Tmt_b
	{0x40, 0x40, 0x40, 0x40, 0x40}, // Cs_b
	{0x40, 0x40, 0x40, 0x40, 0x40} // Pt_b
};

static short * dot[MAX_DOT];

void init_dot(short * address[]) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];
	}
}

void dot_clear() {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0;
	}
	usleep(0); // for Ximulator
}

void dot_write(int number) {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_hexadecimal[number][i];
	}
	usleep(0); // for Ximulator
}

void dot_stack(int n[], int call) {
	int i, j;
	int shift;
	
	for (j=0; n[j]>= 0; j++){
		for(shift = 0; shift <= call; shift++){
			for(i=0; i<MAX_DOT; i++){
				*dot[i] = dot_hexadecimal[n[j]+5][i] >> shift;
			}
		}
	}
	
}

void dot_down_shift(int number, int call, int n[]) { 
	int shift, i;
	dot_write(number+5); 
	for(shift = 0; shift <= call; shift++) {
		for(i=0; i<MAX_DOT; i++){
 			*dot[i] = dot_hexadecimal[number+5][i]>>shift ;
 		}
	usleep(50000);
	}		 
}
