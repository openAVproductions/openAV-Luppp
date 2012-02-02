
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

#ifndef LUPPP_MIXER
#define LUPPP_MIXER

#include <list>
#include <iostream>
#include <memory>

#include "audiotrack.hpp"
#include "porttypes.hpp"

using std::list;

class Top;

class Mixer
{
  public:
    Mixer(Top*);
    
    // need to pass in pointer, not create
    void addTrack(AudioTrack*);
    
    int setSource(int, AudioSource* );
    int addEffect(int, int, Effect* );
    
    void setMasterVolume(float);
    void setMasterRotation(float);
    void setHeadphonesVolume(float);
    
    // returns the UniqueID of an Effect in an AudioTrack
    int getEffectID(int track, int pos);
    
    void process( int nframes, bool,  PortBufferList& portBufferList, CopyBufferList& copyList);
    
  private:
    Top* top;
    
    // volume variables
    float masterVolume;
    float masterRotation;
    float headphonesVolume;
    
    // create named instance for master AudioTrack
    AudioTrack masterTrack;
    list<AudioTrack> audiotrackList;
    
    std::vector<float> inputBuffer;
    
    std::vector<float> outputW;
    std::vector<float> outputX;
    std::vector<float> outputY;
    std::vector<float> outputZ;
    
};

#endif
