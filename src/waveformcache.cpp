
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

#include "waveformcache.hpp"

// static ID counter
int WaveformCache::privateId = 0;

WaveformCache::WaveformCache()
{
  std::cout << "WaveformCache::WaveformCache() Setting numBeats to default" << std::endl;
  // Set ID
  ID = privateId++;
  
  numBeats = 4;
}

int WaveformCache::getID()
{
  return ID;
}
void WaveformCache::setID(int i)
{
  ID = i;
}

void WaveformCache::setBeats(int beats)
{
  if ( beats < 1 )
    beats = 1;
  
  numBeats = beats;
  
  std::cout << "WaveformCache::setBeats(" << beats << ")" << std::endl;
}

int WaveformCache::getBeats()
{
  return numBeats;
}

std::vector<float>* WaveformCache::getPointer()
{
  //std::cout << "WaveformCache::getPointer()  ID: " << ID << "  size = " << buffer.size() << std::endl;
  return &buffer;
}

WaveformCache::~WaveformCache()
{
  std::cout << "WaveformCache::~WaveformCache() Destroyed!" << std::endl;
}

