#include "common.h"

long get_value( void ) { return value; }

void set_value( long new ) { value = new; }

int get_multy( void ) { return multy; }

void set_multy( int new ) { multy = new; }

void do_step( int step ) { value += ( step * multy ) / 100; }
