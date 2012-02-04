
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

#ifndef LUPPP_GUISTATESTORE
#define LUPPP_GUISTATESTORE

#include <list>
#include <vector>
#include <iostream>

// for the shared ptr
#include <memory>

#include "audiobuffer.hpp"
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
    
    AudioBuffer* getNewAudioBuffer();
    AudioBuffer* getWaveformCache(int ID);
    
    void addAudioBufferName(int ID, std::string name);
    
    
    
    std::string getLastDir();
    void setLastDir(std::string s);
    
    void setClipSelectorState(int t,int block, int bufferID);
    
    // all effects should request state from this vector
    vector<EffectState> effectState;
    
    vector<BufferAudioSourceState> bufferAudioSourceState;
    vector<ClipSelectorState> clipSelectorState;
    vector<TrackOutputState> trackoutputState;
    
    vector<std::string> audioBufferNameVector;
    
  private:
    int numTracks;
    
    std::string lastUsedDir;
    
    std::vector< std::shared_ptr<AudioBuffer> > audioBufferVector;
    
};

#endif
