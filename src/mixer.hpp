
#ifndef LUPPP_MIXER
#define LUPPP_MIXER

#include <list>
#include <iostream>
#include <memory>

#include "audiotrack.hpp"

using std::list;

class Top;

class Mixer
{
  public:
    Mixer(Top*);
    
    // need to pass in pointer, not create
    void addTrack();
    
    void addEffect(int, int, Effect* );
    
    void setParameter(int track, int pos, int param, float );
    
    void process( int nframes, float* outBuffer );
    
  private:
    Top* top;
    
    list<AudioTrack> audiotrackList;
    
    std::vector<float> inputBuffer;
    
    std::vector<float> outputW;
    std::vector<float> outputX;
    std::vector<float> outputY;
    std::vector<float> outputZ;
    
};

#endif
