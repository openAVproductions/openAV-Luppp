
#include "audiotrack.hpp"

#include "top.hpp"

int AudioTrack::privateID = 0;

AudioTrack::AudioTrack( Top* t) :
                        source(t, "sample.wav")
{
  top = t;
}

void AudioTrack::process(int nframes, float* buffer)
{
  
  // get audio from source
  source.process(nframes, buffer);
  
}


