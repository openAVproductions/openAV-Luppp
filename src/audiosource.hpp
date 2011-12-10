
#ifndef LUPPP_AUDIOSOURCE
#define LUPPP_AUDIOSOURCE

#include <iostream>

class AudioSource
{
  public:
    AudioSource(){};
    //virtual ~AudioSource();
    
    //virtual void setParameter(int param, float val);
    virtual void process(int nframes, float* buffer) = 0;
};


#endif
