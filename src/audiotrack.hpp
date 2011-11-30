
#ifndef LUPPP_AUDIOTRACK
#define LUPPP_AUDIOTRACK

class Top;

#include <list>
#include <memory>

#include "effect.hpp"
#include "fileaudiosource.hpp"

class AudioTrack
{
  public:
    AudioTrack(Top*);
    
    void process(int nframes, float* ports);
  
  protected:
    int ID;
    static int privateID;
    
    Top* top;
    
    // Source of audio
    FileAudioSource source;
    
    // list of effects
    std::list<Effect*> effects;
};

#endif

