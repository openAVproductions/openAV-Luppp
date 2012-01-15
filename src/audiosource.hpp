
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

#ifndef LUPPP_AUDIOSOURCE
#define LUPPP_AUDIOSOURCE

#include <iostream>

enum AudioSourceType {
  AUDIO_SOURCE_TYPE_BUFFER = 0,
  AUDIO_SOURCE_TYPE_LV2,
  AUDIO_SOURCE_TYPE_FLUIDSYNTH,
  AUDIO_SOURCE_TYPE_WHITENOISE,
};

class AudioSource
{
  public:
    AudioSource(){ID = privateID++;};
    int  getID(){ return ID; }
    virtual void process(int nframes, float* buffer) = 0;
  private:
    int ID;
    static int privateID;
};


#endif
