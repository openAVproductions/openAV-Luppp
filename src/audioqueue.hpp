
#ifndef LUPPP_AUDIOQUEUE
#define LUPPP_AUDIOQUEUE

#include <vector>
#include <string>
#include <iostream>

#include <cstring>
#include <jack/ringbuffer.h>

class AudioQueue
{
  public:
    AudioQueue();
    ~AudioQueue();
    
    void push(int,float*);
    int  writeSamplesTo(float*);
    
    int readSpaceAvailable();
  
  protected:
    jack_ringbuffer_t *buffer;
    float* tempEvent;
};

#endif

