
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

#ifndef LUPPP_OFFLINEWORKER
#define LUPPP_OFFLINEWORKER

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

class Top;

class OfflineWorker
{
  public:
    OfflineWorker(Top*);
    
    int addTrack(int);
    int setTrackSource( int, AudioSourceType type );
    
    int loadAudioBuffer( int, int, std::string name);
    int createNewEffect( int, int, /*EffectType*/ int type );
  
  private:
    Top* top;
    
    std::string toString( int in );
};


#endif
