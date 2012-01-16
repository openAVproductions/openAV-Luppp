
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

#include "mixer.hpp"

#include "top.hpp"

using namespace std;

Mixer::Mixer(Top* t) :
            masterTrack(t,true)
{
  top = t;
  
  // initialize volume levels
  masterVolume     = 0.707f;
  headphonesVolume = 0.707f;
  
  // Top isn't initialized yet, so we just initialize all buffers to 1024
  outputW.resize(1024);
  outputX.resize(1024);
  outputY.resize(1024);
  outputZ.resize(1024);
  
  inputBuffer.resize(1024);
}

void Mixer::addTrack(AudioTrack* trackPtr)
{
  //cout << "Mixer::addTrack() AudioTrack* = " << trackPtr << endl;
  audiotrackList.push_back( *(trackPtr) );
}


int Mixer::setSource(int track, AudioSource* source )
{
  std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
  std::advance(iter, track);
  iter->setSource( source );
  
  return 0;
}

int Mixer::addEffect(int track, int pos,  Effect* eff )
{
  if ( track > 0 && track < audiotrackList.size() )
  {
    std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
    std::advance(iter, track);
    iter->addEffect( pos , eff);
  }
  
  return 0;
}

int Mixer::getEffectID(int track, int pos)
{
  if ( track < audiotrackList.size() )
  {
    //cout << "Mixer::getEffectID() t: " << track << " pos: " << pos << endl;
    std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
    std::advance(iter, track);
    return iter->getEffectID( pos );
  }
  else
  {
    cout << "Mixer::getEffectID() OOB! t: " << track << endl;
  }
  
  return -1;
}

void Mixer::setMasterVolume(float vol)
{
  if ( vol > 1.0 ) vol = 1.0;
  if ( vol < 0.0 ) vol = 0.0;
  masterVolume = vol;
}

void Mixer::setMasterRotation(float rot)
{
  // masterRotation = decimal part of float, regardless of size of int part
  masterRotation = rot - ((int)rot);
}


void Mixer::process(int nframes,bool record, PortBufferList& portBufferList, CopyBufferList& copyBufferList)
{
  if ( nframes > 1024 )
  {
    std::cout << "Mixer::process() nframes too large for internal buffers, reduce it!!" << std::endl;
  }
  
  // we push the audio data to a ringbuffer, to be recorded by GUI, and
  // then passed back to Engine as an AudioBuffer*.
  if ( record )
  {
    top->recordAudioQueue.push(nframes, portBufferList.inputAudio);
  }
  
  // process the entire audio chain here
  std::list<AudioTrack>::iterator iter;
  
  //int c = 0;
  for(iter = audiotrackList.begin(); iter != audiotrackList.end(); iter++ )
  {
    //std::cout << "Mixer:processing  Audiotrack " << c++ << std::endl;
    iter->process( nframes, &portBufferList.inputAudio[0], &outputW[0], &outputX[0], &outputY[0], &outputZ[0]);
  }
  
  // process master effects
  
  
  // create a pointer to each *mixer* owned buffer
  float* outPtrW = &outputW[0];
  float* outPtrX = &outputX[0];
  float* outPtrY = &outputY[0];
  float* outPtrZ = &outputZ[0];
  
  // JACK output port buffers
  float* headphonePort = portBufferList.headphonePfl;
  float* postFaderSendPort = portBufferList.postFaderSend;
  
  // Luppp "extra" output buffers, headphones, postFaderSend
  float* headphonePflBuffer = copyBufferList.headphonePfl;
  float* postFaderSendBuffer = copyBufferList.postFaderSend;
  
  
  bool copyToScopeVector = top->scopeVectorMutex.trylock();
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    // write values to JACK ports
    *portBufferList.outputW++ = *outPtrW * masterVolume;
    *portBufferList.outputX++ = *outPtrX * masterVolume;
    *portBufferList.outputY++ = *outPtrY * masterVolume;
    *portBufferList.outputZ++ = *outPtrZ * masterVolume;
    
    // write headphone & postSend buffers into JACK ports
    *headphonePort++     = *headphonePflBuffer * headphonesVolume;
    *postFaderSendPort++ = *postFaderSendBuffer;
    
    if ( copyToScopeVector )
    {
      // write master output value to scopeVector, to be shown in GUI
      top->scopeVector.at(i) = *outPtrW * masterVolume;
      top->inputScopeVector.at(i) = *portBufferList.inputAudio++;
    }
    
    // write 0.f to *mixer* buffer, and increment
    *outPtrW++ = 0.f;
    *outPtrX++ = 0.f;
    *outPtrY++ = 0.f;
    *outPtrZ++ = 0.f;
    
    // clear headphones & postFaderSend *internal* buffers
    *headphonePflBuffer++  = 0.f;
    *postFaderSendBuffer++ = 0.f;
    
  }
  
  if ( copyToScopeVector )
    top->scopeVectorMutex.unlock();
  
}
