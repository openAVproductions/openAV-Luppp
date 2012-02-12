
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

#include "jackaudiosource.hpp"

#include "top.hpp"

using namespace std;

int JackAudioSource::privateInputPort = 0;

JackAudioSource::JackAudioSource(Top* t)
{
  top = t;
  
  ID = AudioSource::getID();
  
  inputPortNumber = privateInputPort++;
  
  std::cout << "JackAudioSource() ID = " << ID << std::endl;
  
}

void JackAudioSource::process (int nframes, float* buffer )
{
  if ( inputPortNumber < 8 )
  {
    float* tmp = top->state.portBufferList.trackInputs[inputPortNumber];
    
    for ( int i = 0; i < nframes; i++ )
    {
      *buffer++ += *tmp++;
    }
  }
  else
  {
    cout << "JackAudioSource inputPortNumber invalid!" << endl;
  }
}

