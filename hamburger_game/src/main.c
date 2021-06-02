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
static selection_t sel;
static int isThreadCancel = 999;

// for check answer
int call;
int usr_input[5];

int sol[5][5] = {	{0,4,0,-1,-1},
					{0,4,1,0,-1},
					{0,4,3,0,-1},
					{0,3,4,2,0},
					{0,4,1,2,0}
				};

truth_t dot_corr(int inp[], int sol[]) {
	int i;	
	for(i = 0; i < 5; i++) {
		if(inp[i] != sol[i]) {return FALSE; }
	}
	return TRUE;
}

// declare life as a static variable
static int life = 3; // default life = 3
static int level = 1;

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

static truth_t enter = FALSE;

truth_t logic(){
	if( sel.start == 0 ) { game_start_screen(); }
	else if( sel.exit == 1 ) { return FALSE; }
	else if( sel.game == 1 ) { game_mode(); }
	return TRUE;
}

// for thread
static pthread_t p_thread[100];
static int id = 0;
void *count_function(void* i){
	count_down();
	if(!enter){
		life--;
		sel.game = 1;
		fnd_clear();
		dot_clear();
		if((int)i!=0){
			for(int j=(int)i; j>=1; j--){
				pthread_cancel(p_thread[(int)j-1]);
			}
		}
		game_mode();
	}
	//exit(0);
}

void game_start_screen() {
	int i;   char buf;

	led_start();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	if(life<=0){
		clcd_write_string("GAME OVER");
	}else if(level>=5){
		clcd_write_string("CONGRATS! RESTART: S");
	}else{
		clcd_write_string("Welcome to       Hamburger World");
	}
	

	printf("\n");
	printf("*********** Select device **********\n");
	printf("*       press 's' to start game    *\n");
	printf("*       press 'e' to exit game     *\n");
	printf("************************************\n\n");
	scanf("%c", &buf);

	if( buf == 's' ) { 
		life = 3;
		level = 1;
		sel.game = 1;
	}

	if( buf == 'e'){
		sel.exit = 1;
	}
}

void game_mode(){

	if(life <= 0){
		sel.start = 0;
		return;
	}

	if(level <=5){
		life_count(life);

		// initialize usr_input
		for(int i=0; i<5; i++){
			usr_input[i] = -1;
		}
		call = 0;
		enter = FALSE;

		setup_game();

		int count_down_id = pthread_create(&p_thread[id], NULL, count_function, (void*)id);
		id++;

		while(start_game() == TRUE){}

		// user enter q & time over x
		if(enter){
			pthread_cancel(p_thread[id-1]);
			fnd_clear();
			dot_clear();
			sel.game = 1;
		}
		return;

	}else{
		sel.start = 0;
		return;
		
	}
	
}

// show hamburger display
void setup_game(){
	led_up_shift(level);
	clcd_clear_display();
	clcd_write_string("Ready?");
	usleep(1000000);
	clcd_clear_display();
	clcd_write_string("Go!");
	dot_display(sol[level-1]);

	clcd_clear_display();
	clcd_write_string("1.BR 2.LET       3.TMT 4.CHZ 5.PAT");
}

// get user input
truth_t start_game(){
		int key_count, key_value;
		key_count = keyboard_read(&key_value);
	
		if( key_count == 1 && call <= 5 && (key_value == 0 || key_value == 1 || key_value == 2 || key_value == 3 || key_value == 4 || key_value == 5)) {
			if (key_value != 5) {
				usr_input[call] = key_value;
				dot_write(key_value);
				call+=1;
				return TRUE;
		
			} else {
				// if press 'q' end game
				enter =TRUE;
				clcd_clear_display();
				if( dot_corr(usr_input, sol[level - 1]) ) {
					clcd_write_string("You're right!       LEVEL UP");
					level++;
					usleep(1000000);
				
				} else{
					clcd_write_string("You're wrong");
					life--;
					usleep(1000000);
				}
				return FALSE;
			}
		}
		else if(call > 5) {
			clcd_clear_display();
			clcd_write_string("You're wrong");
			life--;
			usleep(1000000);
			return FALSE;
		}
		else{
			clcd_clear_display();
			clcd_write_string("Wrong input");
			usleep(1000000);
			return FALSE;
		}
	
}
