
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
    
    void addEffect( int, Effect* );
    
    void process(int nframes, float* ports);
    
    void setParameter(int, int ,float);
  
  protected:
    int ID;
    static int privateID;
    
    Top* top;
    
    // Source of audio
    AudioSource* source;
    
    
    
    // list of effects
    std::list<Effect*> effects;
};

#endif

