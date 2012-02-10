
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

#ifndef LUPPP_EFFECT
#define LUPPP_EFFECT

#include <iostream>
#include <vector>

class Top;

enum EffectType
{
  EFFECT_REVERB = 0,
  EFFECT_TRANSIENT,
  EFFECT_PARAMETRIC_EQ,
  EFFECT_COMPRESSOR,
  EFFECT_LOWPASS,
  EFFECT_HIGHPASS,
  EFFECT_BEATSMASH,
  EFFECT_LIMITER,
  EFFECT_TRANCEGATE,
  EFFECT_TESTTONES,
  EFFECT_AM_PITCHSHIFT,
};

class Effect
{
  public:
    Effect(Top*, EffectType& t);
    
    int  getID(){ return ID; }
    EffectType getType(){return type;}
    
    virtual void process(int, float*){};
  
  private:
    int ID;
    static int privateID;
    EffectType type;
};

#endif

