
#ifndef LUPPP_AUDIOSOURCE
#define LUPPP_AUDIOSOURCE

#include <iostream>

enum AudioSourceType {
  AUDIO_SOURCE_TYPE_BUFFER = 0,
  AUDIO_SOURCE_TYPE_LV2,
  AUDIO_SOURCE_TYPE_FLUIDSYNTH,
};

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
