
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

#include "bufferaudiosource.hpp"

#include "audiobuffer.hpp"

using namespace std;

BufferAudioSource::BufferAudioSource(Top* t)
{
  top = t;
  ID = AudioSource::getID();
  //cout << "BufferAudioSource() ID = " << ID << endl;
  
  guiUpdateCounter = 0;
}

void BufferAudioSource::process (int nframes, float* buffer )
{
  // get playback variables
  std::list<BufferAudioSourceState>::iterator iter = top->state.bufferAudioSourceState.begin();
  std::advance(iter, ID);
  float index = iter->index;
  float speed;
  
  // get buffer variables
  ClipSelectorState* clipSelState = top->state.getClipSelectorState(ID);
  if ( !clipSelState )
    return;
  
  int playingScene = clipSelState->playing;
  if ( playingScene < 0 )
    return;
  
  std::list<ClipInfo>::iterator clipIter = clipSelState->clipInfo.begin();
  std::advance(clipIter, playingScene);
  int playingBuffer = (*clipIter).bufferID;
  speed = (*clipIter).speed * top->speed;
  //std::cout << "FAS:P()   sceneID: " << playingScene << "   bID = " << playingBuffer << endl;
  
  // get AudioBuffer pointer
  AudioBuffer* bufPtr = top->state.getAudioBuffer(playingBuffer);
  if ( bufPtr == 0 ) {
    // happens until a valid file has been loaded for this 
    //cout << "BufferAudioSource, bufPtr == 0! returning..." << endl;
    return;
  }
  
  // playback the sample
  int size = bufPtr->getPointer()->size();
  std::vector<float>* tmpVector = bufPtr->getPointer();
  
  int FPB = (int) top->samplerate / (top->bpm / 60.0);
  
  //cout << " FPB = " << FPB << "  NumBeats:" << bufPtr->getBeats() << " Playing beat = " << (int)(bufPtr->getBeats() * index) << endl;
  
  int totalFramesWanted = bufPtr->getBeats() * FPB;
  float totalFramesInBuffer = size;
  
  speed = totalFramesInBuffer / totalFramesWanted;
  
  speed = speed * top->speed;
  
  //cout << " Wanted = " << totalFramesWanted << "  Buffer = " << totalFramesInBuffer << "  TopSpeed: " << top->speed <<  "  speed = " << speed << endl;
  
  if ( size == 0 )
    return;
  
  float attackRelease = 1.f;
  
  //cout << "BAS:process() ID = " << ID << "  Size: " << size << " index = " << index << endl;
  for (int i = 0; i < nframes; i++ )
  {
    if ( (int)(index*size) >= size )
    {
      //cout << "BufferAudioSource:proces()  index*size > size!  vectorSize = " << tmpVector->size() << endl;
      index = 0.f;
    }
    
    if ( index * size < 100 )
    {
      attackRelease = (index * size) / 100.f;
    }
    else if ( index * size > size - 100 )
    {
      attackRelease = 1 +   ((size-100) - (index * size))  / 100.f;
    }
    
    // here we multiply in by attackRelease to remove any non-zero endings
    // to make the join over the loop smooth, without a click
    *buffer++  += attackRelease * tmpVector->at( (int)(index * size) );
    index = index + ( speed / size );
  }
  
  cout << "BAS proc() attackRelease = " << attackRelease << endl;
  
  // write index to state, so that next time we read the next samples
  iter->index = index;
  
  if ( guiUpdateCounter < 0 )
  {
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->looperProgress(ID, index ); // index is a float 0 -> 1
    top->toGuiQueue.push(x);
    guiUpdateCounter = 20;
  }
  else
  {
    guiUpdateCounter--;
  }
}
