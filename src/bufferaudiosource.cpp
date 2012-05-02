
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

#include <cmath>

using namespace std;

BufferAudioSource::BufferAudioSource(Top* t)
{
  top = t;
  ID = AudioSource::getID();
  //cout << "BufferAudioSource() ID = " << ID << endl;
  
  // init faust pitch shift variables
  fSamplingFreq = top->samplerate;
  IOTA = 0;
  
  for (int i=0; i<65536; i++)
    fVec0[i] = 0;
  
  semitoneShift = 0.0f;
  windowSize = 1000;
  crossfadeSize = 1000;
  
  for (int i=0; i<2; i++)
    fRec0[i] = 0;
  
  // temp buffer for storing audio
  for ( int i = 0; i < top->bufferSize; i++)
    tmpBuffer.push_back(0.f);
  
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
  
  // pitch algorithm logic
  float deltaPitch = 12 * log ( totalFramesInBuffer / float(totalFramesWanted) ) / log (2);
  //cout << " Wanted = " << totalFramesWanted << "  Buffer = " << totalFramesInBuffer << "  TopSpeed: " << top->speed <<  "  speed = " << speed << "  DeltaPitch: " << deltaPitch << endl;
  
  // here we set the semitone value to the opposite of the resample delta, so it balances out :D
  semitoneShift = -deltaPitch;
  
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
    
    tmpBuffer[i] = 0.f;
    
    // here we multiply in by attackRelease to remove any non-zero endings
    // to make the join over the loop smooth, without a click
    tmpBuffer[i] += attackRelease * tmpVector->at( (int)(index * size) );
    index = index + ( speed / size );
    
    //*buffer++ = tmpBuffer[i];
  }
  
  // write index to state, so that next time we read the next samples
  iter->index = index;
  
  // now pitch shift the audio
  pitchShift( nframes, &tmpBuffer[0], buffer);
  
  
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


void BufferAudioSource::pitchShift(int count, float* input, float* output)
{
  float   fSlow0 = windowSize;
  float   fSlow1 = ((1 + fSlow0) - powf(2,(0.08333333333333333f * semitoneShift)));
  float   fSlow2 = (1.0f / crossfadeSize);
  float   fSlow3 = (fSlow0 - 1);
  float* input0 = &input[0];
  float* output0 = &output[0];
  
  for (int i=0; i<count; i++)
  {
    float fTemp0 = (float)input0[i];
    fVec0[IOTA&65535] = fTemp0;
    fRec0[0] = fmodf((fRec0[1] + fSlow1),fSlow0);
    int iTemp1 = int(fRec0[0]);
    int iTemp2 = (1 + iTemp1);
    float fTemp3 = min((fSlow2 * fRec0[0]), 1.f );
    float fTemp4 = (fSlow0 + fRec0[0]);
    int iTemp5 = int(fTemp4);
    output0[i] = (float)(((1 - fTemp3) * (((fTemp4 - iTemp5) * 
    fVec0[(IOTA-int((int((1 + iTemp5)) & 65535)))&65535]) + ((0 - ((
    fRec0[0] + fSlow3) - iTemp5)) * fVec0[(IOTA-int((iTemp5 & 65535)))
    &65535]))) + (fTemp3 * (((fRec0[0] - iTemp1) * fVec0[(IOTA-int((int(
    iTemp2) & 65535)))&65535]) + ((iTemp2 - fRec0[0]) * fVec0[(IOTA-int((
    iTemp1 & 65535)))&65535]))));
    fRec0[1] = fRec0[0];
    IOTA = IOTA+1;
  }
}



