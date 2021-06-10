//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_hexadecimal[16][MAX_DOT] = {
	{0x1E, 0x3F, 0x3F, 0x3F, 0x1E}, // Br
	{0x0C, 0x12, 0x12, 0x12, 0x0C}, // Let
	{0x0C, 0x1E, 0x12, 0x1E, 0x0C}, // Tmt
	{0x08, 0x1C, 0x3E, 0x1C, 0x08}, // Cs
	{0x1E, 0x3F, 0x3F, 0x3F, 0x1E}, // Pt
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
	usleep(0);
}

void dot_write(int number) {
	int i;
	*dot_type = (short)number;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_hexadecimal[number][i];
	}
	usleep(0); // for Ximulator
}

void dot_display(int arr[8]){
	int i, j;
	for(i=0; i<8;i++){
		for(j =0; j<MAX_DOT; j++){
			if(arr[i] == -1){
				dot_clear();
				return;
			}
			*dot_type = (short) arr[i];
			*dot[j] = dot_hexadecimal[arr[i]][j];
		}
		usleep(1000000);
	}
	dot_clear();
}