
#ifndef LUPPP_OFFLINEWORKER
#define LUPPP_OFFLINEWORKER

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

class Top;

class OfflineWorker
{
  public:
    OfflineWorker(Top*);
    
    int addTrack(int);
    int setTrackSource( int, AudioSourceType type );
    
    int loadAudioBuffer( int, int, std::string name);
    int createNewEffect( int, int, /*EffectType*/ int type );
  
  private:
    Top* top;
    
    std::string toString( int in );
};


#endif
