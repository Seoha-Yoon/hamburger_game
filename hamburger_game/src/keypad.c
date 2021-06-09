//-------| src/keypad.c |-------//
#include "keypad.h"
#include <stdio.h>
#include <string.h>


static short * keypad_out, * keypad_in;

char keyboard[6] = {'1', '2', '3', '4', '5', 'q'};

void init_keypad(short * address_out, short * address_in) {
	keypad_out  = address_out;
	keypad_in = address_in;
}

int keyboard_read(int * key_value){
	int key_count = 0;
	char buf[10];
	scanf("%s", buf);
	key_count = strlen(buf);

	int j;

	if(key_count ==0) return key_count;
	else{
		for(j=0; j<6; j++){
			if(buf[0] ==keyboard[j]) *key_value = j;
		}
		return key_count;
	}
	
}

int input_check(int key_count, int * key_value){
	if(key_count != 1) {return 0;}
	if(*key_value == -1){return 0;}
	return 1;
}