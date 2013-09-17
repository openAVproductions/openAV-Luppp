
#include "debug.hxx"
#include "config.hxx"

#include <cassert>

void luppp_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format, ... )
{
  if ( warnLevel == DEBUG_LEVEL_KILL )
  {
    printf( "[\033[1;31m%s\033[0m] %s:%i: ", NAME, func, line );
  }
  else if ( warnLevel == DEBUG_LEVEL_WARN )
  {
    printf( "[\033[1;33m%s\033[0m] %s:%i: ", NAME, func, line );
  }
  else if ( warnLevel == DEBUG_LEVEL_TEST )
  {
#ifdef BUILD_TESTS
    printf( "[\033[1;33m%s\033[0m] %s:%i: ", NAME, func, line );
#endif
  }
  else // NOTE
  {
    printf( "[\033[1;32m%s\033[0m] %s:%i: ", NAME, func, line );
  }
  printf( "\033[0m" );
  
  if ( format )
  {
    va_list args;
    va_start( args, format );
    vfprintf( stdout, format, args );
    va_end( args );
  }
  printf( "\n" );
  
#ifdef DEBUG_KILL_ON_ERR
  if ( warnLevel == DEBUG_LEVEL_KILL )
  {
    assert(false);
  }
#endif
  
}

