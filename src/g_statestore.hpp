
#ifndef LUPPP_GUISTATESTORE
#define LUPPP_GUISTATESTORE

#include <list>
#include <vector>
#include <iostream>

#include "g_equalizerstate.hpp"
#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"
#include "fileaudiosourcestate.hpp"

using std::vector;

class GuiStateStore
{
  public:
    GuiStateStore();
    
    void addTrack();
    
    EqualizerState eqState;
    
    vector<FileAudioSourceState> fileAudioSourceState;
    vector<ClipSelectorState> clipSelectorState;
    
    vector<TrackOutputState> trackoutputState;
    
  private:
    int numTracks;
    
};

#endif
