
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

#include "trancegate.hpp"

#include "top.hpp"

#include <iostream>

using namespace std;

TranceGate::TranceGate(Top* t, EffectType et) : Effect(t,et)
{
  top = t;
  ID = Effect::getID();
  
  std::cout << "TranceGate() ID = " << ID << std::endl;
  
  active = false;
  
  gateAttack = 12000;
  
  oldBeat = -1;
  
}

void TranceGate::process(int nframes, float *buffer)
{
  //std::cout << "Process: queueActive: " << queueActive << "\tActive: " << active << std::endl;
  
  int framesPerBeat = (int) top->samplerate / (top->bpm / 60.0);
  int newBeat = top->frameNumber / framesPerBeat;
  
  if ( oldBeat != newBeat )
  {
    // new beat, retrigger the AHDSR
    gate = 12000;
    oldBeat = newBeat;
  }
  
  for( int i = 0; i < nframes; i++)
  {
    if ( gate < 0 )
    {
      *buffer = 0.f; // null samples if gate is off
    }
    
    buffer++;
    gate--;
  }
}
