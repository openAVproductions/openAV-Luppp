
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

#ifndef LUPPP_WAVEFORMCACHE
#define LUPPP_WAVEFORMCACHE

#include <iostream>
#include <vector>
#include <glibmm.h>

// essentailly a waveform cache is just a vector of points, equidistant
// (roughly anway), and it can be plotted on the GUI without sample accurate
// reading, so its much faster.

class WaveformCache
{
  public:
    
    WaveformCache();
    ~WaveformCache();
    
    int getID();
    void setID(int);
    
    int  getBeats();
    void setBeats(int);
    
    std::vector<float>* getPointer();
  
  protected:
    
    static int privateId;
    int ID;
    
    int numBeats;
    
    std::vector<float> buffer;
};

#endif

