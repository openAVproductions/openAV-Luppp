#ifndef LUPPP_DENORMALS_H
#define LUPPP_DENORMALS_H

#include <iostream>

#include "debug.hxx"

#ifdef __SSE__
/* On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
   flags to avoid costly denormals */
#ifdef __SSE3__
#include <pmmintrin.h>
inline void AVOIDDENORMALS()
{
  //std::cout << "Denormals: FZ DAZ using SSE3" << std::endl;
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#else
#include <xmmintrin.h>
inline void AVOIDDENORMALS()
{
  //std::cout << "Denormals: FZ" << std::endl;
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}
#endif //__SSE3__

#else
inline void AVOIDDENORMALS()
{
  LUPPP_NOTE( "Denormals: Warning! No protection" );
}
#endif //__SSE__

#endif // LUPPP_DENORMALS_H

