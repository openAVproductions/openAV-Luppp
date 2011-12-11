
#ifndef LUPPP_TOP
#define LUPPP_TOP

#include <iostream>

#include <glibmm.h>

#include "rtqueue.hpp"

#include "statestore.hpp"
#include "audiofileloader.hpp"

// the Top class is a portal trough which the rest of the program can
// commnunicate. It allows the JACK thread send messages to other threads,
// and vice versa. It also aids as a point where various utility classes
// can communicate
class Top
{
  public:
    Top();
    
    void addTrack();
    
    int bufferSize;
    int samplerate;
    int bpm;
    
    AudioFileLoader audioFileLoader;
    StateStore state;
    
    Glib::Dispatcher* guiDispatcher;
    RtQueue toGuiQueue;
    
    RtQueue toEngineQueue;
};

#endif
