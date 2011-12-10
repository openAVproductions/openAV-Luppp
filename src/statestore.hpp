
#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <cmath>
#include <iostream>

#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"
#include "fileaudiosourcestate.hpp"

class StateStore
{
  public:
    StateStore();
    
    void addTrack();
    
    void setVolume(int,float);
    
    TrackOutputState* getAudioSinkOutput(int);
    
    
    std::list<FileAudioSourceState> fileAudioSourceState;
    std::list<ClipSelectorState> clipSelectorState;
    
    std::list<TrackOutputState> trackoutputState;
    
  private:
    int numTracks;
    
    bool trackCheck(int);
    
};

#endif
