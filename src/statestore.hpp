
#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <cmath>
#include <iostream>

class Top;

#include "audiobuffer.hpp"

#include "effect.hpp"

struct EffectState;

#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"

#include "bufferaudiosourcestate.hpp"

class StateStore
{
  public:
    StateStore(Top*);
    
    int midib1,midib2,midib3;
    
    int beatSmash;
    
    float effectValues[8];
    
    int getNumTracks(){return numTracks;}
    int getNumEffects(){return numEffects;}
    
    // these functions accept pointers to instances and then insert the objects
    // into the lists that Engine uses for its operation
    void addTrack(BufferAudioSourceState*, ClipSelectorState*, TrackOutputState*);
    void addEffect(int t, int p, Effect* e);
    
    // to be called whenever we want a new "EffectState" added to the list
    void addEffectState(int ID);
    
    int setMute(int,int);
    int setSolo(int,int);
    int setRec (int,int);
    int setPan(int,float);
    int setPanZ(int,float);
    int setVolume(int,float);
    
    void setPluginActive(int, int);
    void setPluginParameter(int,int,float);
    
    void setAudioBuffer(AudioBuffer*);
    AudioBuffer* getAudioBuffer(int ID);
    
    void clipSelectorActivateClip(int,int);
    
    // sets loaded on that clip
    void setClipSelectorState(int t,int block, int bufferID);
    
    EffectState*       getEffectState(int);
    TrackOutputState*  getAudioSinkOutput(int);
    ClipSelectorState* getClipSelectorState(int);
    
    
    // these lists will be private, and functions as above will be used
    // to set values, while State* get() methods will provide all data
    std::list<BufferAudioSourceState> bufferAudioSourceState;
    std::list<ClipSelectorState> clipSelectorState;
    std::list<TrackOutputState> trackoutputState;
    
    std::list<AudioBuffer> audiobufferList;
    std::list<Effect> effectList;
    
    std::list<EffectState*> effectStateList;
    
  private:
    Top* top;
    int numTracks;
    int numEffects;
    
    inline bool trackCheck(int);
    inline bool effectCheck(int);
    
};

#endif
