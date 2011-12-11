
#ifndef LUPPP_AUDIOSINK
#define LUPPP_AUDIOSINK

#include <iostream>

class AudioSink
{
  public:
    AudioSink(){};
    //virtual ~AudioSink();
    
    //virtual void setParameter(int param, float val);
    virtual void process(int , float*, float*, float *, float *, float *){};
};


#endif
