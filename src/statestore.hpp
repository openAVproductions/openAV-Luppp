
#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <iostream>

#include "fileaudiosourcestate.hpp"

class StateStore
{
  public:
    StateStore();
    
    void addTrack();
    
    std::list<FileAudioSourceState> fileAudioSourceState;
    
};

#endif
