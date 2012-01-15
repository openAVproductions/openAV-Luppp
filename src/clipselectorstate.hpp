
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


#ifndef LUPPP_CLIPSELECTORSTATE
#define LUPPP_CLIPSELECTORSTATE

#include <vector>

enum ClipState {
  CLIP_STATE_EMPTY          = 1,
  CLIP_STATE_LOADED         = 2,
  CLIP_STATE_PLAYING        = 3,
  CLIP_STATE_PLAY_QUEUED    = 4,
  CLIP_STATE_RECORDING      = 5,
  CLIP_STATE_MASTER_TRACK   = 6,
};

// this struct should contain *all* information needed for buffer playback,
// as well as enough info for the APC to be updated.
struct ClipInfo {
  ClipInfo()
  {
    bufferID = -1;
    
    hasBuffer = false;
    
    numBeats = 8;
    speed = 1.f;
  }
  
  int bufferID;
  bool hasBuffer;
  
  int numBeats;
  float speed;
};

struct ClipSelectorState
{
  ClipSelectorState()
  {
    ID = -1;
    
    // initial values for playback block
    playing   = -1;
    queued    = -1;
    recording = -1;
    
    for(int i = 0; i < 10; i++)
      clipInfo.push_back( ClipInfo() );
  }
  
  int ID;
  
  int playing;
  int queued;
  int recording;
  
  std::list<ClipInfo> clipInfo;
};

#endif
