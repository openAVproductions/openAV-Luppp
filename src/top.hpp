
#ifndef LUPPP_TOP
#define LUPPP_TOP

#include <iostream>
#include "rtqueue.hpp"

#include  "jackclient.hpp"

// the Top class is a portal trough which the rest of the program can
// commnunicate. It allows the JACK thread send messages to other threads,
// and vice versa.
// 
// There is a mutex on messages *sent* to JACK, as its a
// single write, single read queue. This will not affect RT performance
// as the RT thread will never try write a message to itself.
class Top
{
  public:
    Top();
    
    JackClient* jackClient;
    int bufferSize;
    int samplerate;
    
  private:
    RtQueue toJackQueue;
};

#endif
