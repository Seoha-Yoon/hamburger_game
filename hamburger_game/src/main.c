//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

static int fd;
static int map_counter = 0;
static void * map_data[100];
static seclection_t sel; 


int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	led = mapper(IEB_LED, PROT_WRITE);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
	}
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);
	keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	keypad_in = mapper(IEB_KEY_R, PROT_READ);
	
	init_led(led);
	init_dot(dot);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);
	init_keypad(keypad_out, keypad_in);
	
	sel.all = 0;
	while( logic() == TRUE ) {	}
	
	unmapper();
	close(fd);
	return 0;
}

short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

truth_t logic() {
	if( sel.all == 0 ) { select_mode(); }
	else if( sel.exit == 1 ) { return FALSE; }
	else { input_mode(); }
	return TRUE;
}

void select_mode() {
	int i;  char buf[100];
	char clcd_str[20] = "";
	
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();
	
	printf("\n");
	printf("*********** Select device **********\n");
	printf("*   l (LED)       d (Dot Matrix)   *\n");
	printf("*   f (FND)       c (CLCD)         *\n");
	printf("*   a (All devices)                *\n");
	printf("*       press 'e' to exit program  *\n");
	printf("************************************\n\n");
	scanf("%s", buf);
	
	for( i=0; i<strlen(buf); i++ ) {
		if( buf[i] == 'l' ) { sel.led = 1; }
		else if( buf[i] == 'd' ) { sel.dot  = 1; }
		else if( buf[i] == 'f' ) { sel.fnd  = 1; }
		else if( buf[i] == 'c' ) { sel.clcd = 1; }
		else if( buf[i] == 'e' ) { sel.exit = 1;  break; }
		else if( buf[i] == 'a' ) { 
			sel.all = 0xFF;  sel.exit = 0;  break;
		}
	}
	
	if( sel.led  == 1 ) { strcat(clcd_str, "LED "); }
	if( sel.dot  == 1 ) { strcat(clcd_str, "Dot "); }
	if( sel.fnd  == 1 ) { strcat(clcd_str, "FND "); }
	if( sel.clcd == 1 ) { strcat(clcd_str, "CLCD"); }
	clcd_write_string(clcd_str);
	
}

void input_mode() {
	int key_count, key_value;
	char clcd_str[20];
	//key_count = keypad_read(&key_value);
	key_count = keyboard_read(&key_value);
	
	if( key_count == 1 ) {
		if( sel.led  == 1 ) { led_bit(key_value); }
		if( sel.dot  == 1 ) { dot_write(key_value); }
		if( sel.fnd  == 1 ) { fnd_write(key_value, 7); }
		if( sel.clcd == 1 ) { 
			sprintf(clcd_str, "%#04x            ", key_value);
			clcd_set_DDRAM(0x40);
			clcd_write_string(clcd_str);
		}
	}
	else if( key_count > 1 ) {
		sel.all = 0;
	}
	usleep(0); // simulator update
}

