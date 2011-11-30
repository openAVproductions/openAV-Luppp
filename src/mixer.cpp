
#include "mixer.hpp"

#include "top.hpp"

Mixer::Mixer(Top* t)
{
  top = t;
  
  std::cout << "Resizing Mixer output buffer to " << 1024 << std::endl;
  outputBuffer.resize(1024);
  
  AudioTrack track(top);
  audiotrackList.push_back( track );
}

void Mixer::process(int nframes, float* ports)
{
  // process the entire audio chain here
  std::list<AudioTrack>::iterator iter;
  
  for(iter = audiotrackList.begin(); iter != audiotrackList.end(); iter++ )
  {
    iter->process( nframes, &outputBuffer[0] );
  }
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    *ports++ = outputBuffer[i];
  }
  
}
