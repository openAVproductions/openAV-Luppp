
#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <iostream>

#include "fileaudiosource.hpp"

class StateStore
{
  public:
    StateStore();
  
  private:
    std::list<FileAudioSourceState> fileAudioSourceState;
    
};

#endif
