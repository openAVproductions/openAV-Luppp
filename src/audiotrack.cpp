
#include "audiotrack.hpp"

#include "top.hpp"

int AudioTrack::privateID = 0;

AudioTrack::AudioTrack( Top* t) :
                        source("sample.wav")
{
  top = t;
}

void AudioTrack::addEffect( int pos,  Effect* eff )
{
  std::list<Effect*>::iterator iter =  effects.begin();
  
  std::advance(iter, pos);
  
  effects.insert( iter , eff);
}

void AudioTrack::process(int nframes, float* buffer)
{
  
  // get audio from source
  source.process(nframes, buffer);
  
}


