
#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <cmath>
#include <iostream>

class Top;

#include "audiobuffer.hpp"

#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"
#include "fileaudiosourcestate.hpp"

class StateStore
{
  public:
    StateStore(Top*);
    
    void addTrack();
    
    void setMute(int,int);
    void setSolo(int,int);
    void setRec (int,int);
    void setPan(int,float);
    void setPanZ(int,float);
    void setVolume(int,float);
    
    void setAudioBuffer(AudioBuffer*);
    AudioBuffer* getAudioBuffer(int ID);
    
    void clipSelectorQueue(int,int);
    
    // sets loaded on that clip
    void setClipSelectorState(int t,int block, int bufferID);
    
    TrackOutputState*  getAudioSinkOutput(int);
    ClipSelectorState* getClipSelectorState(int);
    
    
    // these lists will be private, and functions as above will be used
    // to set values, while State* get() methods will provide all data
    std::list<FileAudioSourceState> fileAudioSourceState;
    std::list<ClipSelectorState> clipSelectorState;
    std::list<TrackOutputState> trackoutputState;
    std::list<AudioBuffer> audiobufferList;
    
  private:
    Top* top;
    int numTracks;
    
    bool trackCheck(int);
    
};

#endif
