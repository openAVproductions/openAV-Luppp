
#ifndef LUPPP_BEATSMASH
#define LUPPP_BEATSMASH

#include <iostream>
#include <deque>

class Top;
#include "effect.hpp"

class BeatSmash : public Effect
{
  public:
    BeatSmash(Top*);
    
    void process(int nframes, float *L);
  
  protected:
    int ID;
    Top* top;
    
    int active;
    int queueActive;
    
    int nframeBeatCounter;
    
    // like a vector, but access from front & back is equally fast
    std::deque<float> audioBuffer;
    
};

#endif

