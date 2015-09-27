#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LEN 30

long get_value( void );
void set_value( long );
int  get_multy( void );
void set_multy( int );
void do_step( int step );

#endif
