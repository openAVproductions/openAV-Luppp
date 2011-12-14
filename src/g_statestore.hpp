
#ifndef LUPPP_GUISTATESTORE
#define LUPPP_GUISTATESTORE

#include <list>
#include <vector>
#include <iostream>

#include "effectstate.hpp"

#include "g_equalizerstate.hpp"
#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"
#include "bufferaudiosourcestate.hpp"

using std::vector;

class GuiStateStore
{
  public:
    GuiStateStore();
    
    void addTrack();
    
    float cutoff;
    
    std::string getLastDir();
    void setLastDir(std::string s);
    
    void setClipSelectorState(int t,int block, int bufferID);
    
    EqualizerState eqState;
    
    vector<EffectState> effectState;
    
    vector<BufferAudioSourceState> bufferAudioSourceState;
    vector<ClipSelectorState> clipSelectorState;
    vector<TrackOutputState> trackoutputState;
    
    vector<std::string> audioBufferNameVector;
    
  private:
    int numTracks;
    
    std::string lastUsedDir;
    
};

#endif
