//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_hexadecimal[16][MAX_DOT] = {
	{0x3C, 0x7E, 0x7E, 0x7E, 0x3C}, // Br
	{0x18, 0x24, 0x24, 0x24, 0x18}, // Let
	{0x3C, 0x7E, 0x66, 0x7E, 0x3C}, // Tmt
	{0x08, 0x1C, 0x3E, 0x1C, 0x08}, // Cz
	{0x18, 0x3C, 0x3C, 0x3C, 0x18}, // Pt
};

static short * dot[MAX_DOT];
static short * dot_type;

void init_dot(short * address[], short* address_dot_type) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];
	}
	dot_type = address_dot_type;
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
	*dot_type = (short)number;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_hexadecimal[number][i];
	}
	usleep(0); // for Ximulator
}

void dot_display(){
	int i, j;
	int arr[]={0, 1, 0};
	for(i=0; i<sizeof(arr)/sizeof(int);i++){
		for(j =0; j<MAX_DOT; j++){
			*dot_type = (short) arr[i];
			*dot[j] = dot_hexadecimal[arr[i]][j];
		}
		usleep(500000);
	}
}