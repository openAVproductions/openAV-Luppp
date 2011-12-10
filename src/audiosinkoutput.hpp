
#ifndef LUPPP_AUDIOSINKOUTPUT
#define LUPPP_AUDIOSINKOUTPUT

#include <cmath>
#include <vector>
#include <iostream>

#include "top.hpp"

#include "audiosink.hpp"

class AudioSinkOutput : public AudioSink
{
  public:
    AudioSinkOutput(Top*);
    
    void setParameter(int, float);
    void process(int nframes, float* in, float* W, float *X, float *Y, float *Z);
  
  private:
    int ID;
    static int privateID;
    
    Top* top;
    
    float pan;
    float panZ;
    float volume;
    float logVolume;
    
    // convienience functions
    void setPan(float);
    void setVolume(float);
};

#endif

