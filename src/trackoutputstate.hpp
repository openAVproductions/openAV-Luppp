
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

#ifndef LUPPP_TRACKOUTPUTSTATE
#define LUPPP_TRACKOUTPUTSTATE

struct TrackOutputState
{
  TrackOutputState()
  {
    ID = -1;
    selected = -1;
    selectedDevice = 0;
    
    volume = 0.f;
    pan    = 0.f;
    panZ   = 0.f;
    
    sends = 0.f;
    
    rms = 0.f;
    falloff = 0.f;
    
    pflEnable = false;
    
    mute = solo = recEnable = false;
  }
  
  int ID;
  
  // for parameter control
  bool selected;
  int selectedDevice;
  
  float sends;
  
  float volume;
  float pan;
  float panZ;
  
  float rms;
  float falloff;
  
  bool mute, solo, recEnable;
  bool pflEnable;
};

#endif
