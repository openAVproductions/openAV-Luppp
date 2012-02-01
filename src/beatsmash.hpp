
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

#ifndef LUPPP_BEATSMASH
#define LUPPP_BEATSMASH

#include <iostream>
#include <deque>

class Top;
#include "effect.hpp"

class BeatSmash : public Effect
{
  public:
    BeatSmash(Top*, EffectType);
    
    void process(int nframes, float *L);
  
  protected:
    int ID;
    Top* top;
    
    int active;
    int queueActive;
    
    int smashIndex;
    
    int nframeBeatCounter;
    
    // like a vector, but access from front & back is equally fast
    std::deque<float> audioBuffer;
    
};

#endif

