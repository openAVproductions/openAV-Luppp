
#ifndef LUPPP_TOP
#define LUPPP_TOP

#include <iostream>

#include <glibmm.h>

#include "rtqueue.hpp"
#include "audioqueue.hpp"

#include "statestore.hpp"

class Controller;
class JackClient;
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
    void setJackPointer(JackClient* j){jackClient = j;};
    
    int bufferSize;
    int samplerate;
    int bpm;
    int frameNumber;
    
    float speed;
    
    Glib::Mutex scopeVectorMutex;
    std::vector<float> scopeVector;
    std::vector<float> inputScopeVector;
    
    Controller* controller; 
    OfflineWorker* offlineWorker;
    StateStore state;
    
    Glib::Dispatcher* guiDispatcher;
    RtQueue toGuiQueue;
    
    RtQueue toEngineQueue;
    
    RtQueue toEngineEmptyEventQueue;
    
    AudioQueue recordAudioQueue;
    
    JackClient* jackClient;
};

#endif
