
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

#ifndef LUPPP_PORTTYPES
#define LUPPP_PORTTYPES

struct PortBufferList
{
  PortBufferList()
  {
    outputW = 0;
    outputX = 0;
    outputY = 0;
    outputZ = 0;
    
    headphonePfl  = 0;
    postFaderSend = 0;
    
    inputAudio = 0;
  }
  // audio outputs
  float* outputW;
  float* outputX;
  float* outputY;
  float* outputZ;
  
  float* headphonePfl;
  float* postFaderSend;
  
  // audio inputs
  float* inputAudio; // for recording
  
  // for processing JACK input per track
  float* trackInputs[8];
  
  // for master return
  float* masterReturn[4];
};

struct CopyBufferList
{
  CopyBufferList()
  {
    headphonePfl  = 0;
    postFaderSend = 0;
    
    W = 0;
    X = 0;
    Y = 0;
    Z = 0;
  }
  
  float* headphonePfl;
  float* postFaderSend;
  
  float* W;
  float* X;
  float* Y;
  float* Z;
};

#endif
