// Made by plaonn(plaonn@cau.ac.kr)

/****************************************************************************\
|*                              Header                                      *|
\****************************************************************************/
#include <Ximulator.h>

/****************************************************************************\
|*                              Comment                                     *|
\****************************************************************************/

/****************************************************************************\
|*                              Macro                                       *|
\****************************************************************************/

/****************************************************************************\
|*                              Typedef                                     *|
\****************************************************************************/

/****************************************************************************\
|*                              Enum                                        *|
\****************************************************************************/

/****************************************************************************\
|*                              Struct                                      *|
\****************************************************************************/

/****************************************************************************\
|*                              Global                                      *|
\****************************************************************************/

/*********************************************|  Static Memory Map |*********/
static int rand_count = 0;
static int rand_data[SIM_MAX_RAND];
static char mem_open_flag = 0;
static void * mem_map_address[SIM_SMM_CNT] = {0, };


/*********************************************|  CLCD              |*********/

static clcd_cmd_t clcd_cmd = {0};
static unsigned long clcd_data = 0;
static char sim_clcd_dd[SIM_CLCD_DD_RAM_MAX_LINE][SIM_CLCD_DD_RAM_MAX_CHAR];
static short sim_clcd_cursor = 0, sim_clcd_offset = 0;
static char sim_clcd_shift_direction = 1, sim_clcd_shift_target = 0; // entry mode
static char sim_clcd_onoff = 1;
static char sim_clcd_lines = 2;
static char sim_clcd_not_supported = 0;



/*********************************************|  FND               |*********/

static unsigned long fnd_data[8];

/*********************************************|  Dots & LED        |*********/

static unsigned long dots_data[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned long led_data = 0;
static unsigned long dots_type = 999;

static unsigned long keypad_out =0;
static unsigned long keypad_in = 0;
/****************************************************************************\
|*                              Simulator                                   *|
\****************************************************************************/

/*********************************************|  CLCD              |*********/
static void sim_clcd_clear(){
	memset(sim_clcd_dd, ' ', sizeof(sim_clcd_dd));
	sim_clcd_cursor = 0;
	sim_clcd_offset = 0;
}
static void sim_clcd_return(){
	sim_clcd_cursor = 0;
	sim_clcd_offset = 0;
}
static void sim_clcd_mode(){
	sim_clcd_shift_direction = clcd_cmd.ID ? 1 : -1;
	sim_clcd_shift_target = clcd_cmd.S;
}
static void sim_clcd_control(){
	sim_clcd_onoff = clcd_cmd.D;
}
static void sim_clcd_shift(){
	if(clcd_cmd.SC) { sim_clcd_offset -= clcd_cmd.RL ? 1 : -1; }
	else { sim_clcd_cursor += clcd_cmd.RL ? 1 : -1; }
}
static void sim_clcd_function(){
	if(clcd_cmd.DL == 0) { sim_clcd_not_supported = 1; }
	if(clcd_cmd.F  == 1) { sim_clcd_not_supported = 1; }
	sim_clcd_lines = clcd_cmd.N ? 2 : 1;
}
static void sim_clcd_setCC(){
	sim_clcd_not_supported = 1;
}
static void sim_clcd_setDD(){
	sim_clcd_cursor = clcd_cmd.DD_Address;
}

static void sim_clcd_ctrl(){

	if ( clcd_cmd.CMD != 0 ) {

		if	   (clcd_cmd.clrDisp  == 1)	{ sim_clcd_clear(); } 
		else if(clcd_cmd.retHome  == 1)	{ sim_clcd_return(); }
		else if(clcd_cmd.entrySet == 1) { sim_clcd_mode(); }
		else if(clcd_cmd.dispCtrl == 1) { sim_clcd_control(); }
		else if(clcd_cmd.shift    == 1) { sim_clcd_shift(); }
		else if(clcd_cmd.funcSet  == 1) { sim_clcd_function(); }
		else if(clcd_cmd.set_CC   == 1) { sim_clcd_setCC(); }
		else if(clcd_cmd.set_DD   == 1) { sim_clcd_setDD(); }
		
		clcd_cmd.CMD = 0;
	}
	else if ( clcd_data != 0 ) {

		int short ddram_char = sim_clcd_cursor & 0x3F;
		int short ddram_line = sim_clcd_cursor >> 6;
		sim_clcd_dd[ddram_line][ddram_char] = clcd_data;
		sim_clcd_cursor += sim_clcd_shift_direction;
		if(sim_clcd_shift_target) { sim_clcd_offset += sim_clcd_shift_direction; }
		
		clcd_data = 0;
	}

}
static void sim_clcd_view(){
	int line, pos, pos_shifted;
	puts("\n\t+-----[ CLCD ]-----+");
	if(sim_clcd_not_supported) {
		puts("Ximulator : Some function(s) are not supported");
	}
	else {
		for ( line = 0; line < sim_clcd_lines; line++){ 
			fputs("\t| ", stdout);
			for ( pos = 0; pos < SIM_CLCD_MAX_CHAR; pos++ ){
				if(sim_clcd_onoff) {
					pos_shifted = pos + sim_clcd_offset;
					if( (pos_shifted < 0) || (pos_shifted >= SIM_CLCD_DD_RAM_MAX_CHAR) ) { putchar(' '); }
					else {
						if( (sim_clcd_dd[line][pos_shifted] >= ' ') && (sim_clcd_dd[line][pos_shifted] <= '~') ) {
							putchar(sim_clcd_dd[line][pos_shifted]);
						}
						else { putchar('?'); }
					}
				}
				else { putchar(' '); }
			}
			fputs(" |\n", stdout);
		}
	}
	fputs("\t+------------------+", stdout);
}

/*********************************************|  FND               |*********/
static void sim_fnd_view() {
	// A
	putchar('\n');
	fputs("|F| ", stdout);
	for (int i=7; i>=0; i--) {
		putchar(' ');
		if( fnd_data[i] & 0x01 ) {
			putchar('-');
			putchar('-');
		}
		else {
			putchar(' ');
			putchar(' ');
		}
		putchar(' ');
		putchar(' ');
	}
	putchar('\n');
	// F & B
	fputs("| | ", stdout);
	for (int i=7; i>=0; i--) {
		if( fnd_data[i] & 0x20 ) {
			putchar('|');
		}
		else {
			putchar(' ');
		}
		putchar(' ');
		putchar(' ');
		if( fnd_data[i] & 0x02 ) {
			putchar('|');
		}
		else {
			putchar(' ');
		}
		putchar(' ');
	}
	putchar('\n');
	// G
	fputs("|N| ", stdout);
	for (int i=7; i>=0; i--) {
		putchar(' ');
		if( fnd_data[i] & 0x40 ) {
			putchar('-');
			putchar('-');
		}
		else {
			putchar(' ');
			putchar(' ');
		}
		putchar(' ');
		putchar(' ');
	}
	putchar('\n');
	// E & C
	fputs("| | ", stdout);
	for (int i=7; i>=0; i--) {
		if( fnd_data[i] & 0x10 ) {
			putchar('|');
		}
		else {
			putchar(' ');
		}
		putchar(' ');
		putchar(' ');
		if( fnd_data[i] & 0x04 ) {
			putchar('|');
		}
		else {
			putchar(' ');
		}
		putchar(' ');
	}
	putchar('\n');
	// D & Dot
	fputs("|D| ", stdout);
	for (int i=7; i>=0; i--) {
		putchar(' ');
		if( fnd_data[i] & 0x08 ) {
			putchar('-');
			putchar('-');
		}
		else {
			putchar(' ');
			putchar(' ');
		}
		putchar(' ');
		if( fnd_data[i] & 0x80 ) {
			putchar('.');
		}
		else {
			putchar(' ');
		}
	}
	putchar('\n');
}

/*********************************************|  Dots & LED        |*********/
static void sim_dots_view() {
	char color[18]="";

	switch(dots_type){
		case 0x00:
			strcat(color,"\033[33m@\033[0m");
			break;
		case 0x01:
			strcat(color,"\033[1;42;92m@\033[0m");
			break;
		case 0x02:
			strcat(color,"\033[0;31m@\033[0m");
			break;
		case 0x03:
			strcat(color,"\033[1;43;33m@\033[0m");
			break;
		case 0x04:
			strcat(color,"\033[1;41;30m@\033[0m");
			break;
	}
	puts("\t[ Dots ]\t[ LED ]");
	// 
	for(int i=0; i<7; i++) {
		fputs("\t ", stdout);
		for(int j=0; j<5; j++) {
			if( dots_data[j] & (0x40 >> i) ) {
				if(dots_type == 999){
					putchar('@');
				}else{
					printf("%s",color);
				}
			}
			else {
				putchar('.');
			}
		}
		fputs("  \t   ", stdout);
		if( led_data & (1 << i) ) {
				putchar('.');
			}
			else {
				putchar('@');
		}
		putchar('\n');
	}
	fputs("\t        \t   ", stdout);
	if( led_data & (1 << 7) ) {
			putchar('.');
		}
		else {
			putchar('@');
	}
	putchar('\n');
	
}


/*********************************************|  Simulator Body    |*********/
static void simulator(){
	system("clear");
	puts(" =====|      X i m u l a t o r      |=====");   
	puts(" =====| the X-Hyper320TKU Simulator |=====");
	
	sim_clcd_ctrl();
	sim_clcd_view();
	sim_fnd_view();
	sim_dots_view();
}


/****************************************************************************\
|*                              Wrapper                                     *|
\****************************************************************************/
int get_random_number() {
	if(rand_count == 0) { srand(time(NULL)); }
	else if(rand_count >= SIM_MAX_RAND) {
		puts("[Ximulator] :\nCan't open() again.");
		exit(-1);
	}
	return rand_data[rand_count++] = rand();
}

int unget_random_number(int number) {
	int i;
	for(i=0; i<rand_count; i++) {
		if(number == rand_data[i]) {
			rand_data[i] = rand_data[--rand_count];
			return 0;
		}
	}
	return -1;
}

int __wrap_usleep(useconds_t useconds){
	simulator();
	return __real_usleep(useconds);
}


int __wrap_open(const char *path, int oflag, ... ){
	if( strcmp(path, "/dev/mem") != 0 ) {
		puts("[Ximulator] :\nCan't open file.\nPlease do open() with \"/dev/mem\"");
		exit(-1);
	}
	if ( !( (oflag & O_RDWR) && (oflag & O_SYNC) ) ) {
		puts("[Ximulator] :\nCan't open file.\nPlease do open() with correct options.");
		exit(-1);
	}
	
	mem_open_flag = 1;

	/* initializing */
	memset(sim_clcd_dd, ' ', sizeof(sim_clcd_dd));
	memcpy(sim_clcd_dd[0], "X-HYPER320 TKU", strlen("X-HYPER320 TKU"));
	memcpy(sim_clcd_dd[1], "Embedded Linux_", strlen("Embedded Linux_"));
	
	simulator();
	return get_random_number();
}

int __wrap_close(int fildes){
	if ( unget_random_number(fildes) < 0 ) {
		puts("[Ximulator] :\nCan't close file.\nPlease do close() with valid fd.");
		exit(-1);
	}
	mem_open_flag = 0;
	simulator();
	return 0;
}


void* __wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off){
	
	if(mem_open_flag) {
		if ( (addr == NULL) && (prot & PROT_WRITE || prot & PROT_READ) && (flags & MAP_SHARED) ) { 
			void * return_address;
			
				 if( off == SIM_MMAP_SMM[ 0] ) { mem_map_address[ 0] = return_address = &fnd_data[0]; }
			else if( off == SIM_MMAP_SMM[ 1] ) { mem_map_address[ 1] = return_address = &fnd_data[1]; }
			else if( off == SIM_MMAP_SMM[ 2] ) { mem_map_address[ 2] = return_address = &fnd_data[2]; }
			else if( off == SIM_MMAP_SMM[ 3] ) { mem_map_address[ 3] = return_address = &fnd_data[3]; }
			else if( off == SIM_MMAP_SMM[ 4] ) { mem_map_address[ 4] = return_address = &fnd_data[4]; }
			else if( off == SIM_MMAP_SMM[ 5] ) { mem_map_address[ 5] = return_address = &fnd_data[5]; }
			else if( off == SIM_MMAP_SMM[ 6] ) { mem_map_address[ 6] = return_address = &fnd_data[6]; }
			else if( off == SIM_MMAP_SMM[ 7] ) { mem_map_address[ 7] = return_address = &fnd_data[7]; }

			else if( off == SIM_MMAP_SMM[ 8] ) { mem_map_address[ 8] = return_address = &dots_data[0]; }
			else if( off == SIM_MMAP_SMM[ 9] ) { mem_map_address[ 9] = return_address = &dots_data[1]; }
			else if( off == SIM_MMAP_SMM[10] ) { mem_map_address[10] = return_address = &dots_data[2]; }
			else if( off == SIM_MMAP_SMM[11] ) { mem_map_address[11] = return_address = &dots_data[3]; }
			else if( off == SIM_MMAP_SMM[12] ) { mem_map_address[12] = return_address = &dots_data[4]; }

			else if( off == SIM_MMAP_SMM[13] ) { mem_map_address[13] = return_address = &clcd_cmd; }
			else if( off == SIM_MMAP_SMM[14] ) { mem_map_address[14] = return_address = &clcd_data; }

			else if( off == SIM_MMAP_SMM[15] ) { mem_map_address[15] = return_address = &led_data; }
			
			else if( off == SIM_MMAP_SMM[16] ) { mem_map_address[16] = return_address;}
			else if( off == SIM_MMAP_SMM[17] ) { mem_map_address[17] = return_address;}

			else if( off == SIM_MMAP_SMM[18] ) { mem_map_address[18] = return_address = &dots_type; }
			else { 
				printf("[Ximulator] :\nThe device [%#010x] is not supported.\n", (int)off); 
				exit(-1);
			}
			
			return return_address;
		}
		else { 
			puts("[Ximulator] :\nCan't map memory.\nPlease check mmap() options.");
			exit(-1);
		}
	}
	else { 
		puts("[Ximulator] :\nCan't map memory.\nPlease open() before mmap().(1)"); 
		exit(-1);
	}
	return MAP_FAILED;
}

int __wrap_munmap(void *addr, size_t len){
	int i;

	if(addr) {
		for(i=0; i<SIM_SMM_CNT; i++) {
			if( addr == mem_map_address[i] ) {
				simulator();
				mem_map_address[i] = 0;
				return 0;
			}
		}
	}
	
	puts("[Ximulator] :\nCan't unmap memory.\nPlease mmap() before munmap().(2)"); 
	exit(-1);
}


/****************************************************************************\
|*                              Test Code                                   *|
\****************************************************************************/