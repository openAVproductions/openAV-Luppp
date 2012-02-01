
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

#include "beatsmash.hpp"

#include "top.hpp"
#include "effectstate.hpp"

BeatSmash::BeatSmash(Top* t, EffectType et) : Effect(t,et)
{
  top = t;
  ID = Effect::getID();
  
  //std::cout << "BeatSmash() ID = " << ID << std::endl;
  
  active = false;
  queueActive = false;
  
  smashIndex = 0;
  
  audioBuffer.resize(22050, 0.f);
}

void BeatSmash::process(int nframes, float *L)
{
  //std::cout << "Process: queueActive: " << queueActive << "\tActive: " << active << std::endl;
  EffectState* state = top->state.getEffectState(ID);
  
  bool active = state->active;
  int delaySize = (int)(state->values[0] * 8);
  float wetDry = state->values[1];
  
  // work out delay lengths for beat, quater beat, half beat etc
  int framesPerBeat = (int) top->samplerate / (top->bpm / 60.0);
  
  int smashLoopSize = (1 + delaySize) * (framesPerBeat / 8.f);
  
  float* buffer = L;
  
  for( int i = 0; i < nframes; i++)
  {
    
    if (active)
    {
      // audioBuffer contains the last beat of samples
      //
      // OLDEST                                       Newest
      //   |                                            |
      //   +============================================+
      //   |                      |                     |
      //   0         audioBuffer.size() / 2             audioBuffer.size()
      
      if ( smashIndex > audioBuffer.size() || smashIndex > smashLoopSize )
      {
        smashIndex = 0;
      }
      
      // write smash loop into buffer
      *buffer = (*buffer * wetDry) + (1-wetDry) * audioBuffer.at(smashIndex++);
      buffer++;
    }
    else
    {
      // update contents
      audioBuffer.push_back( L[i] );
      if ( audioBuffer.size() > smashLoopSize )
      {
        audioBuffer.pop_front();
      }
    }
  }
}
