
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
    
    void process(int nframes, float* in, float* W, float *X, float *Y, float *Z);
  
  private:
    int ID;
    static int privateID;
    
    Top* top;
};

#endif

