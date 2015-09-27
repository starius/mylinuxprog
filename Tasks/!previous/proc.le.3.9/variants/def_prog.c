#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "common.h"

#define FIRST_CHAR (int)'0'

static int get_proc( void ) {
   char dev[ 80 ];
   int df;
   sprintf( dev, "/proc/%s", NAME_NODE );
   if( ( df = open( dev, O_RDONLY ) ) < 0 ) {
      sprintf( dev, "/proc/%s/%s", NAME_DIR, NAME_NODE );
      if( ( df = open( dev, O_RDONLY ) ) < 0 )
         printf( "open file error: %m\n" ), exit( EXIT_FAILURE );
   }
   return df;
}

static int def_proc( char *path ) {
   int df;
   if( ( df = open( path, O_RDWR | O_CREAT | O_TRUNC, 0660 ) ) < 0 )
      printf( "open file %s error: %m\n", path ), exit( EXIT_FAILURE );
   return df;
}
