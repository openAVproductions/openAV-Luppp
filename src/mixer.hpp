
#ifndef LUPPP_MIXER
#define LUPPP_MIXER

#include <iostream>

class Top;

class Mixer
{
  public:
    Mixer(Top*);
    
    void process( int nframes, float* portList );
    
  private:
    Top* top;
    
};

#endif
