
#ifndef LUPPP_AUDIOTRACK
#define LUPPP_AUDIOTRACK

class Top;

#include <list>
#include <memory>

#include "effect.hpp"
#include "audiosink.hpp"
#include "audiosource.hpp"

class AudioTrack
{
  public:
    AudioTrack(Top*);
    
    void addEffect( int, Effect* );
    
    void process(int nframes, float* ports, float*,float*,float*,float*);
    
    void setParameter(int, int ,float);
  
  protected:
    int ID;
    static int privateID;
    
    Top* top;
    
    // Audio elements
    AudioSink*    sink;
    AudioSource*  source;
    
    // list of effects
    std::list<Effect*> effects;
    
    std::vector<float> trackBuffer;
};

#endif

