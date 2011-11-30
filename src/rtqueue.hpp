
#ifndef RTQUEUE
#define RTQUEUE

#include <vector>
#include <string>
#include <iostream>

#include <cstring>
#include <jack/ringbuffer.h>

#include "engineevent.hpp"

class RtQueue
{
  public:
    RtQueue();
    ~RtQueue();
    
    void push(EngineEvent *);
    EngineEvent* pull();
  
  protected:
    jack_ringbuffer_t *buffer;
    EngineEvent* tempEvent;
};

#endif

