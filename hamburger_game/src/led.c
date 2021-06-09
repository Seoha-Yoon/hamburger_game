#include "led.h"

static short * led;

void init_led(short * address) {
	led = address;
}

void led_level(int level) {
	
	int i;
	for( i=8; i<=level+8 ; i++ ) {
		*led = (short)((unsigned long)0xFF00 >> i);
	}
	usleep(0);
}


void led_clear() {
	*led = (short)~0;
}
