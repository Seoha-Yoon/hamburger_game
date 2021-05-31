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

// for thread
void *count_function(){
	count_down();
}

static int fd;
static int map_counter = 0;
static void * map_data[100];
static selection_t sel;

// for check answer
int call = 6;
int usr_input[6] = {-1, -1, -1, -1, -1, -1};

int sol[3][5] = { {0,4,0,-1, -1},
		{0,4,1,0,-1},
		{0,4,1,2,0}
		};

truth_t dot_corr(int inp[], int sol[]) {
	int i;	
	for(i = 0; i < 5; i++) {
		if(inp[i] != sol[i]) {return FALSE; }
	}
	return TRUE;
}


int main() {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in, * dots_type;
	
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
	dots_type = mapper(IEB_DOT_TYPE, PROT_WRITE);
	
	init_led(led);
	init_dot(dot, dots_type);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);

	sel.start == 0;
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

truth_t logic(){
	if( sel.start == 0 ) { game_start_screen(); }
	else if( sel.exit == 1 ) { return FALSE; }
	return TRUE;
}

void game_start_screen() {
	int i;   char buf;
	char clcd_str[20] = "";

	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	clcd_write_string("Welcome to       Hamburger World");
	
	printf("\n");
	printf("*********** Select device **********\n");
	printf("*       press 's' to start game    *\n");
	printf("*       press 'e' to exit game    *\n");
	printf("************************************\n\n");
	scanf("%c", &buf);

	if( buf == 's' ) { 
		game_mode();
	}

	if( buf == 'e'){
		sel.exit = 1;
	}
}

void game_mode(){
	int key_count, key_value;

	setup_game();

	pthread_t c_down;
	int count_down_id = pthread_create(&c_down, NULL, count_function, NULL);

	while(start_game() == TRUE ){ }

	dot_clear();
	pthread_cancel(c_down);
	
	key_count = keyboard_read(&key_value);
	
	if( key_count == 1 ) {
		
	}
	else if( key_count > 1 ) {
		sel.start = 0;
	}
	usleep(0); // simulator update
}

// show hamburger display
void setup_game(){
	clcd_clear_display();
	clcd_write_string("Ready?");

	dot_display();

	clcd_clear_display();
	clcd_write_string("1.LET 2.BR       3.TMT 4.CHZ 5.PAT");
	life_count(3);	
}

// get user input
truth_t start_game(){
	
	int key_count, key_value;
	key_count = keyboard_read(&key_value);

	if( key_count == 1 && call < 7) {
		if (key_value != 5) {
			usr_input[6-call] = key_value;
			dot_write(key_value);
			call-=1;
			return TRUE;
		
		} else {
			if( dot_corr(usr_input, sol[0]) ) {
				led_blink_all();
			} else led_all();
			return FALSE;
		}
	}
	else{
		return FALSE;
	}
}



