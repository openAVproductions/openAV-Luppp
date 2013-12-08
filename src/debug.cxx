/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "debug.hxx"
#include "config.hxx"

#include <cassert>

void luppp_debug( int warnLevel, const char* name, const char* file, const char* func, int line,
                  const char* format, ... )
{
  if ( warnLevel == DEBUG_LEVEL_ERROR )
  {
    printf( "[\033[1;31m%s\033[0m] %s:%i: ", name, func, line );
  }
  else if ( warnLevel == DEBUG_LEVEL_WARN )
  {
    printf( "[\033[1;33m%s\033[0m] %s:%i: ", name, func, line );
  }
  else if ( warnLevel == DEBUG_LEVEL_TEST )
  {
#ifdef BUILD_TESTS
    printf( "[\033[1;33m%s\033[0m] %s:%i: ", name, func, line );
#endif
  }
  else // NOTE
  {
    printf( "[\033[1;32m%s\033[0m] %s:%i: ", name, func, line );
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
  
}

