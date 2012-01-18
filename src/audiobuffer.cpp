
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

#include "audiobuffer.hpp"

// static ID counter
int AudioBuffer::privateId = 0;

AudioBuffer::AudioBuffer()
{
  std::cout << "AudioBuffer::AudioBuffer() Setting numBeats to default" << std::endl;
  // Set ID
  ID = privateId++;
  
  numBeats = 4;
}

int AudioBuffer::getID()
{
  return ID;
}

void AudioBuffer::setBeats(int beats)
{
  if ( beats < 1 )
    beats = 1;
  
  numBeats = beats;
  
  std::cout << "AudioBuffer::setBeats(" << beats << ")" << std::endl;
}

int AudioBuffer::getBeats()
{
  return numBeats;
}

std::vector<float>* AudioBuffer::getPointer()
{
  //std::cout << "AudioBuffer::getPointer()  ID: " << ID << "  size = " << buffer.size() << std::endl;
  return &buffer;
}

AudioBuffer::~AudioBuffer()
{
  std::cout << "AudioBuffer::~AudioBuffer() Destroyed!" << std::endl;
}

