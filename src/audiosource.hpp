
#ifndef LUPPP_AUDIOSOURCE
#define LUPPP_AUDIOSOURCE

#include <iostream>

class AudioSource
{
  public:
    AudioSource(){ID = privateID++;};
    int  getID(){ return ID; }
    virtual void process(int nframes, float* buffer) = 0;
  private:
    int ID;
    static int privateID;
};


#endif
