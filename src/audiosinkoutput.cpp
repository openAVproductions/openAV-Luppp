
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

#include "audiosinkoutput.hpp"

#include "jackclient.hpp"

int AudioSinkOutput::privateID = 0;

AudioSinkOutput::AudioSinkOutput(Top* t)
{
  ID = privateID++;
  
  std::cout << "AudioSinkOutput() ID = " << ID << std::endl;
  
  amplitudeSendCounter = 0;
  
  top = t;
}

void AudioSinkOutput::process(int nframes, float* in, float *W, float *X, float *Y, float* Z)
{
  TrackOutputState* state = top->state.getAudioSinkOutput(ID);
  
  //std::cout << "AudioSinkOutput::process() vol = " << state->volume << " pan = " << state->pan << std::endl;
  
  float logVolume = state->volume;
  float pan = -1 * state->pan; // invert pan, way the algorithm works!
  float panZ = 0.f;
  
  // azimuth
  float cosAzimuth  = cos( pan * 3.14 );
  float sinAzimuth  = sin( pan * 3.14 );
  
  // elevation
  float cosElevation  = cos( panZ );
  float sinElevation  = sin( panZ );
  
  /*
  ; calculate cos and sin of 
  ; time-varying angles 
  kca = cos(kone)  ; azimuth
  ksa = sin(kone) 
  kcb = cos(ktwo)  ; elevation
  ksb = sin(ktwo)

  ; B-format encoding equations

  ax = a5 * kca * kcb 
  ay = a5 * ksa * kcb 
  az = a5 * ksb 
  aw = a5 * 0.707
  */
  
  float tmp;
  
  // write to the reverb send JACK port
  // this is to cater for effects like the zita-rev1 ambisonic reverb:
  // it sends a mono *post fader* signal, and the returned B-format
  // gets mixed into the master bus return.
  float* postFaderSend = top->jackClient->getPostFaderSendVector();
  
  // get headphones PFL buffer
  float* headphonePfl = top->jackClient->getHeadphonePflVector();
  
  for( int i = 0; i < nframes; i++)
  {
    // += so we don't overwrite the previous tracks!
    tmp = in[i] * 0.707 * logVolume;
    
    if ( !state->mute ) // only write when not muted
    {
      // write to postFaderSend buffer
      *postFaderSend++ += tmp * state->sends;
      
      W[i] += tmp;
      X[i] += in[i] * cosAzimuth * cosElevation * logVolume;
      Y[i] += in[i] * sinAzimuth * cosElevation * logVolume;
      Z[i] += in[i] * sinElevation * logVolume;
    }
    
    if ( state->pflEnable )
    {
      *headphonePfl++ += tmp;
    }
    
    // clear the internal track buffer, postFaderSend & headphonePfl
    in[i] = 0.f;
    
    if ( maxAmplitude < tmp )
      maxAmplitude = tmp;
  }
  
  amplitudeSendCounter -= nframes;
  
  //std::cout << "SinkOutput " << ID << " processing now" << std::endl;
  
  if ( amplitudeSendCounter < 0 )
  {
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setTrackRms( ID, maxAmplitude );
    top->toGuiQueue.push(x);
    
    maxAmplitude = 0.f;
    amplitudeSendCounter = top->samplerate / 10;
  }
}
