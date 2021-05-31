//-------| src/keypad.c |-------//
#include "keypad.h"
#include <stdio.h>
#include <string.h>


static short * keypad_out, * keypad_in;

char keyboard[7] = {'1', '2', '3', '4', '5', 'q', 's'};

void init_keypad(short * address_out, short * address_in) {
	keypad_out  = address_out;
	keypad_in = address_in;
}


int keyboard_read(int * key_value){
	int key_count = 0;
	char buf[10];
	scanf("%s",buf);
	key_count = strlen(buf);
	int i, j;
	if(key_count ==0){
		return key_count;
	}
	else{
		for(i=0; i<key_count; i++){
			for(j=0; j<7; j++){
				if(buf[i] ==keyboard[j]) *key_value = j;
			}
		}
		return key_count;
	}
}