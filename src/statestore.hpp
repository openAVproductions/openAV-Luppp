
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LUPPP_STATESTORE
#define LUPPP_STATESTORE

#include <list>
#include <cmath>
#include <iostream>

class Top;

#include "audiobuffer.hpp"

#include "effect.hpp"
#include "automove.hpp"

struct EffectState;

#include "trackoutputstate.hpp"
#include "clipselectorstate.hpp"

#include "bufferaudiosourcestate.hpp"

#include "porttypes.hpp"

class StateStore
{
  public:
    StateStore(Top*);
    
    // current buffers in the engine: in statestore to allow eg JackAudioSource to
    // select JACK port input without it passing trough Mixer & AudioTrack
    PortBufferList portBufferList;
    
    int midib1,midib2,midib3;
    
    float globalUnit;
    AutoMoveType globalUnitType;
    
    int getNumTracks(){return numTracks;}
    int getNumEffects(){return numEffects;}
    
    // these functions accept pointers to instances and then insert the objects
    // into the lists that Engine uses for its operation
    void addTrack(BufferAudioSourceState*, ClipSelectorState*, TrackOutputState*);
    void addEffect(int t, int p, Effect* e);
    
    // to be called whenever we want a new "EffectState" added to the list
    void addEffectState(EffectState*);
    
    int setMute(int,int);
    int setSolo(int,int);
    int setRec (int,int);
    int setPan(int,float);
    int setPanZ(int,float);
    int setVolume(int,float);
    int setSend(int t, int send, float value);
    
    void setPluginActive(int, int);
    void setPluginParameter(int,int,float);
    
    void setPluginGlobalUnit(int ID, int onOff);
    
    void setAudioBuffer(AudioBuffer*);
    AudioBuffer* getAudioBuffer(int ID);
    
    
    // for live selecting of buffers
    void clipSelectorQueueClip(int t, int b);
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
