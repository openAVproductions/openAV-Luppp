
#ifndef LUPPP_DEBUG_HXX
#define LUPPP_DEBUG_HXX

#include <stdio.h>
#include <stdarg.h>

/* Example usage
LUPPP_NOTE( "%s", "MessageHere" );
LUPPP_WARN( "%s", "MessageHere" );
LUPPP_KILL( "%s", "MessageHere" );
*/

enum DEBUG_LEVEL {
  DEBUG_LEVEL_NOTE = 0,
  DEBUG_LEVEL_WARN,
  DEBUG_LEVEL_ERROR,
  DEBUG_LEVEL_TEST
};

void luppp_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format = 0, ... );


#define LUPPP_DSP( format, args... ) luppp_debug( DEBUG_LEVEL_NOTE, " DSP ", "", "", 0, format, ## args )

#define LUPPP_NOTE( format, args... ) luppp_debug( DEBUG_LEVEL_NOTE, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define LUPPP_WARN( format, args... ) luppp_debug( DEBUG_LEVEL_WARN, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define LUPPP_ERROR( format, args... ) luppp_debug( DEBUG_LEVEL_ERROR, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )

// only gets printed if #definde BUILD_TESTS
#define LUPPP_PRINT_TEST( format, args... ) luppp_debug( DEBUG_LEVEL_DEBUG_ONLY, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )

#endif

