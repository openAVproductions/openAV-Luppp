
#include "mixer.hpp"

#include "top.hpp"

Mixer::Mixer(Top* t)
{
  top = t;
  
}

void Mixer::process(int nframes, float* ports)
{
  // process the entire audio chain here
  
  
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    //*ports++ = 
  }
  
}
