
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

using namespace std;

BeatSmash::BeatSmash(Top* t, EffectType et) : Effect(t,et)
{
  top = t;
  ID = Effect::getID();
  
  //std::cout << "BeatSmash() ID = " << ID << std::endl;
  
  active = false;
  queueActive = false;
  
  writeIndex = 0;
  
  smashIndex = 0;
  
  audioBuffer.resize(22050, 0.f);
}

void BeatSmash::process(int nframes, float* buffer)
{
  //std::cout << "Process: queueActive: " << queueActive << "\tActive: " << active << std::endl;
  EffectState* state = top->state.getEffectState(ID);
  
  // AutoMove feature, range scale the unit's value here
  float globalChange = 0.f;
  switch ( top->state.globalUnitType )
  {
    case AUTOMOVE_TYPE_UP:        globalChange = 1 - state->values[0]; break;
    case AUTOMOVE_TYPE_DOWN:      globalChange =     state->values[0]; break;
    default: break;
  }
  
  bool active = state->active;
  
  float finalValue = state->values[0];
  if ( state->globalUnit ) // overwrite old value with globalUnit effected one
  {
    finalValue = state->values[0] + globalChange * top->state.globalUnit;
  }
  
  // update GUI
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setPluginParameter(ID, -1, 0, finalValue);
  top->toGuiQueue.push(x);
  
  int delaySize = 1 + (int)(finalValue * 3.8);
  
  // scale the delaySize to a musically meaningful number
  if      ( delaySize == 1 )
  {
    delaySize = 1;
  }
  else if ( delaySize == 2 )
  {
    delaySize = 2;
  }
  else if ( delaySize == 3 )
  {
    delaySize = 4;
  }
  else if ( delaySize == 4 )
  {
    delaySize = 8;
  }
  
  float wetDry = state->values[1];
  
  // work out delay lengths for beat, quater beat, half beat etc
  int framesPerBeat = (int) top->samplerate / (top->bpm / 60.0);
  
  int smashLoopSize = (1.f / delaySize ) * (framesPerBeat);
  
  //cout << "Delay Size = " << delaySize << "   smashLoopSize " << smashLoopSize << endl;
  
  if ( !active )
  {
    queueActive = smashLoopSize;
    writeIndex  = 0;
    smashIndex = 0;
    return;
  }
  
  for( int i = 0; i < nframes; i++)
  {
    // record until we have loop to smash
    if ( queueActive > 0 )
    {
      //cout << "BufferSize = " << audioBuffer.size() << " write index = " << writeIndex << endl;
      if ( writeIndex < audioBuffer.size() )
      {
        audioBuffer.at(writeIndex++) = buffer[i];
        queueActive--;
      }
      else
        queueActive = false; // we've got the loop, we're not "active"-not queuing audio to become active
    }
    else
    {
      if ( smashIndex > audioBuffer.size() || smashIndex > smashLoopSize )
      {
        smashIndex = 0;
      }
    
      // write smash loop into buffer
      if ( smashIndex < audioBuffer.size() )
      {
        *buffer = (*buffer * wetDry) + (1-wetDry) * audioBuffer.at(smashIndex++);
      }
      else
      {
        smashIndex = 0;
      }
      buffer++;
      
    }
  }
}
