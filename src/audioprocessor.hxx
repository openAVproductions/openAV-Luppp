
#ifndef LUPPP_AUDIO_PROCESSOR_H
#define LUPPP_AUDIO_PROCESSOR_H

#include <stdio.h>

class AudioProcessor
{
  public:
    AudioProcessor(){}
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    virtual void process(int nframes, Buffers* buffers){printf("AudioProcessor::process() not derived\n");}
    
    /// set main mix, 0-1
    virtual void setMaster(float value){}
    
    /// set sidechain mix, 0-1
    virtual void setSidechain(float value){}
    
    /// set post sidechain mix, 0-1
    virtual void setPostSidechain(float value){}
    
    /// set reverb mix, 0-1
    virtual void setReverb(float value){}
    
    virtual ~AudioProcessor(){};
};

#endif // LUPPP_AUDIO_PROCESSOR_H

