
#include "mixer.hpp"

#include "top.hpp"

Mixer::Mixer(Top* t)
{
  top = t;
  
  // Top isn't initialized yet, so we just initialize all buffers to 1024
  outputW.resize(1024);
  outputX.resize(1024);
  outputY.resize(1024);
  outputZ.resize(1024);
  
  inputBuffer.resize(1024);
}

void Mixer::addTrack()
{
  audiotrackList.push_back( *(new AudioTrack(top)) );
}

void Mixer::setParameter(int track, int pos, int param, float val )
{
  std::list<AudioTrack>::iterator iter;
  std::advance(iter, track);
  iter->setParameter(pos, param, val);
}


void Mixer::process(int nframes, float* outBuffer)
{
  if ( nframes > 1024 )
  {
    std::cout << "Mixer::process() nframes too large for internal buffers, reduce it!!" << std::endl;
  }
  
  // process the entire audio chain here
  std::list<AudioTrack>::iterator iter;
  
  for(iter = audiotrackList.begin(); iter != audiotrackList.end(); iter++ )
  {
    iter->process( nframes, &inputBuffer[0], &outputW[0], &outputX[0], &outputY[0], &outputZ[0] );
  }
  
  float* outPointer = &outputW[0];
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    // copy value
    *outBuffer++ = *outPointer;
    
    // write 0.f to buffer, and increment
    *outPointer++ = 0.f;
    inputBuffer[i] = 0.f;
    
  }
  
}
