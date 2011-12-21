
#ifndef LUPPP_TOP
#define LUPPP_TOP

#include <iostream>

#include <glibmm.h>

#include "rtqueue.hpp"

#include "statestore.hpp"

class Controller;
class OfflineWorker;

// the Top class is a portal trough which the rest of the program can
// commnunicate. It allows the JACK thread send messages to other threads,
// and vice versa. It also aids as a point where various utility classes
// can communicate
class Top
{
  public:
    Top();
    void initialize();
    
    void addTrack();
    
    int bufferSize;
    int samplerate;
    int bpm;
    
    Glib::Mutex scopeVectorMutex;
    std::vector<float> scopeVector;
    
    Controller* controller;
    OfflineWorker* offlineWorker;
    StateStore state;
    
    Glib::Dispatcher* guiDispatcher;
    RtQueue toGuiQueue;
    
    RtQueue toEngineQueue;
    
    RtQueue toEngineEmptyEventQueue;
};

#endif
