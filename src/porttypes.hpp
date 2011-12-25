
#ifndef LUPPP_PORTTYPES
#define LUPPP_PORTTYPES

struct PortBufferList
{
  PortBufferList()
  {
    outputW = 0;
    outputX = 0;
    outputY = 0;
    outputZ = 0;
    
    inputAudio = 0;
  }
  // audio outputs
  float* outputW;
  float* outputX;
  float* outputY;
  float* outputZ;
  
  // audio inputs
  float* inputAudio;
};

#endif
