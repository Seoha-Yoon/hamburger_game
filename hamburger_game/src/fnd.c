//-------| src/fnd.c |-------//
#include "fnd.h"

static unsigned short fnd_hexadecimal[16] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x67, // 9
	0x77, // A
	0x7C, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71  // F
};

static short * fnd[MAX_FND];

void init_fnd(short * address[]) {
	int i;
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = address[i];
	}
}

void fnd_clear() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0;
	}
}

void fnd_all() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0xFF;
	}
}

void fnd_write(int hexadecimal, int fnd_num) {
	*fnd[fnd_num] = fnd_hexadecimal[hexadecimal];
}

void fnd_hexa_number(unsigned long number) {
	int i;
	for(i=0; i<MAX_FND; i++){
		fnd_write( (short)(number & 0xF), i);
		number = number >> 4;
	}
}

int fnd_check(){
	if(*fnd[0] ==0 && *fnd[1]==0) return 1;
	else return 0;
}

void count_down() {
	int i=9,j,k=2;
	for(j=29; j>=0; j--){
			*fnd[0] = fnd_hexadecimal[i];
			*fnd[1] = fnd_hexadecimal[k];
			usleep(500000);
			i--;
			if (i<0) {
				i=9;
				k--;
				*fnd[1] = fnd_hexadecimal[k];
				if (k<0) {
					*fnd[0] = fnd_hexadecimal[0];
					*fnd[1] = fnd_hexadecimal[0];
				}
			}
	}
}

void life_count(int n) {
	*fnd[7] = fnd_hexadecimal[n];
}