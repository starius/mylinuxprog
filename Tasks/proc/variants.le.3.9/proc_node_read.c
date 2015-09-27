#ifndef VARIANT
#define VARIANT 0
#endif

static ssize_t proc_node_read( char *buffer, char **start, off_t off,
                               int count, int *eof, void *data ) {
   unsigned long len = 0;
   LOG( "read: %d (buffer=%p, off=%ld, start=%p)", count, buffer, off, *start );

#if VARIANT == 0

   // тупо копируем весь буфер, пока return не станет <= off
   memcpy( buffer, buf_msg, length );
   LOG( "copy bytes: %ld", length );
   len = length;

#elif VARIANT == 1

   // ... на 1 шаг раньше ( return == off ) установлен eof
   if( off < length ) {
      memcpy( buffer, buf_msg, length );
      LOG( "copy bytes: %d", length );
      len = length;
   }
   *eof = off + count >= length ? 1 : 0;

#elif VARIANT == 2

   // 1-но копирование всего буфера на 1-м запросе
   len = length;
   if( 0 == off ) {
      memcpy( buffer, buf_msg, length );
      LOG( "copy bytes: %d", length );
   }
   *eof = off + count >= length ? 1 : 0;

#elif VARIANT == 3

   // копирование только count байт на каждом запросе - аккуратно меняем return!
   if( off >= length ) {
      *eof = 1;
   }
   else {
      int cp;
      len = length - off;
      cp = min( count, len );
      memcpy( buffer + off, buf_msg + off, cp );
      LOG( "copy bytes: %d", cp );
      len = off + cp;
      *eof = off + cp >= length ? 1 : 0;
   }

#else 
#error недопустимый VARIANT !
#endif

   LOG( "return bytes: %ld%s", len, *eof != 0 ? " ... EOF" : "" );
   return len;
};
