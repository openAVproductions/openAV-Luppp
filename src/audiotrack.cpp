
#include "audiotrack.hpp"

#include "top.hpp"

#include "fileaudiosource.hpp"
#include "fluidsynthaudiosource.hpp"

#include "beatsmash.hpp"
#include "ladspahost.hpp"

#include "trackoutputstate.hpp"
#include "audiosinkoutput.hpp"

int AudioTrack::privateID = 0;

AudioTrack::AudioTrack( Top* t )
{
  top = t;
  
  ID = privateID++;
  
  std::cout << "Creating AudioTrack ID: " << ID << std::endl;
  
  trackBuffer.resize(top->bufferSize);
  
  if ( ID == 0 || ID == 1 )
  {
    source = new FileAudioSource(t, "sample.wav");
    addEffect( 1, new BeatSmash(top) );
    addEffect( 1, new LadspaHost(top, EFFECT_LOWPASS, top->samplerate) );
  }
  else
  {
    source = new FluidSynthAudioSource(t, "example.sf2");
  }
  
  sink = new AudioSinkOutput(t);
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


