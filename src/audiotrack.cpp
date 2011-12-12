
#include "audiotrack.hpp"

#include "top.hpp"
#include "ladspahost.hpp"

// only needed for implementation
#include "fileaudiosource.hpp"
#include "audiosinkoutput.hpp"

#include "trackoutputstate.hpp"

int AudioTrack::privateID = 0;

AudioTrack::AudioTrack( Top* t )
{
  top = t;
  
  ID = privateID++;
  
  source = new FileAudioSource(t, "sample.wav");
  
  sink = new AudioSinkOutput(t);
  
  trackBuffer.resize(1024);
  
  if ( ID == 0 )
    addEffect( 1, new LadspaHost(top, EFFECT_LOWPASS, top->samplerate) );
}

void AudioTrack::addEffect( int pos,  Effect* eff )
{
  std::list<Effect*>::iterator iter =  effects.begin();
  std::advance(iter, pos);
  
  effects.insert( iter , eff);
}

void AudioTrack::setParameter(int pos, int param, float f)
{
  std::cout << "SetParameter(" << pos << ", " << param << ", " << f << ")    Effects = " << &effects << std::endl;
  
  //std::cout << "Effects empty? " << effects.empty() << std::endl;
  
  //std::list<Effect*>::iterator iter =  effects.begin();
  //std::advance(iter, pos);
  //(*iter)->setParameter(param, f);
}

void AudioTrack::process(int nframes, float* buffer, float* W, float* X, float* Y, float* Z)
{
  //std::cout << "AudioTrack::process()" << std::endl;
  
  TrackOutputState* state = top->state.getAudioSinkOutput(ID);
  if ( state->mute ) { return; }
  
  // get audio from source
  source->process(nframes, &trackBuffer[0]);
  
  for(std::list<Effect*>::iterator iter = effects.begin(); iter != effects.end(); iter++ )
  {
    (*iter)->process( nframes, &trackBuffer[0] );
  }
  
  // do volume & panning
  sink->process(nframes, &trackBuffer[0], W, X, Y, Z);
  
}


