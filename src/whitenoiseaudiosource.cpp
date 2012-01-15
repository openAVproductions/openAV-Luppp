
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

#include "whitenoiseaudiosource.hpp"

#include "top.hpp"

#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;

WhiteNoiseAudioSource::WhiteNoiseAudioSource(Top* t)
{
  top = t;
  
  srand( (unsigned)time(0) );
  
  ID = AudioSource::getID();
  
  tmpOutput.resize( top->bufferSize );
  dummyOutput.resize( top->bufferSize );
  
}

void WhiteNoiseAudioSource::process (int nframes, float* buffer )
{
  /*
  // get playback variables
  std::list<WhiteNoiseAudioSourceState>::iterator iter = top->state.fileAudioSourceState.begin();
  std::advance(iter, ID);
  */
  
  for ( int i = 0; i < nframes; i++ )
  {
    *buffer++ += ((float)rand()) / RAND_MAX;
  }
}
