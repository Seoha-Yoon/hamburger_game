//-------| lib/dot.h |-------//
#ifndef	DOT_H
#define	DOT_H 


#include <unistd.h> // usleep()


#define MAX_DOT 5


void init_dot(short * address[], short* address_dot_type);
void dot_clear();
void dot_write(int number) ;
void dot_down_shift(int number, int call, int n[]) ;

#endif
