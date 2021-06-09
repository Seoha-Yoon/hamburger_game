//-------| lib/led.h |-------//
#ifndef	LED_H
#define	LED_H 


#include <unistd.h> // usleep()


void init_led(short * address);
void led_level(int level);
void led_clear();

#endif
