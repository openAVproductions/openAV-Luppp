
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
    
    headphonePfl  = 0;
    postFaderSend = 0;
    
    inputAudio = 0;
  }
  // audio outputs
  float* outputW;
  float* outputX;
  float* outputY;
  float* outputZ;
  
  float* headphonePfl;
  float* postFaderSend;
  
  // audio inputs
  float* inputAudio;
};

struct CopyBufferList
{
  CopyBufferList()
  {
    headphonePfl  = 0;
    postFaderSend = 0;
  }
  
  float* headphonePfl;
  float* postFaderSend;
};

#endif
