
#include "debug.hxx"
#include "config.hxx"

#include <cassert>

void luppp_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format, ... )
{
  if ( warnLevel == DEBUG_LEVEL_KILL )
  {
    printf( "[\033[1;31m%s\033[0m] %s : %s : %i ", NAME, file, func, line );
#ifdef DEBUG_KILL_ON_ERR
    assert(false);
#endif
  }
  else if ( warnLevel == DEBUG_LEVEL_WARN )
  {
    printf( "[\033[1;33m%s\033[0m] %s : %s : %i ", NAME, file, func, line );
  }
  else // NOTE
  {
    printf( "[\033[1;32m%s\033[0m] %s : %s : %i ", NAME, file, func, line );
  }
  
  if ( format )
  {
    va_list args;
    va_start( args, format );
    printf( format, args );
    va_end( args );
  }
  printf( "\033[0m\n" );
}

