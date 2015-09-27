#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *msg = "test\n";

int main() {
   register int len = strlen( msg );
   write( STDOUT_FILENO, msg, len );
   exit( 7 );
};
