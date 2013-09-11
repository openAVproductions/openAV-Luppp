
#ifndef LUPPP_DEBUG_HXX
#define LUPPP_DEBUG_HXX

#include <stdio.h>
#include <stdarg.h>

enum DEBUG_LEVEL {
  DEBUG_LEVEL_NOTE = 0,
  DEBUG_LEVEL_WARN,
  DEBUG_LEVEL_KILL
};

void luppp_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format = 0, ... );

#define LUPPP_NOTE( format, args... ) luppp_debug( DEBUG_LEVEL_NOTE, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define LUPPP_WARN( format, args... ) luppp_debug( DEBUG_LEVEL_WARN, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )
#define LUPPP_KILL( format, args... ) luppp_debug( DEBUG_LEVEL_KILL, NAME, __FILE__, __FUNCTION__, __LINE__, format, ## args )

#endif

