
#ifndef LUPPP_OFFLINEWORKER
#define LUPPP_OFFLINEWORKER

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

class Top;

class OfflineWorker
{
  public:
    OfflineWorker(Top*);
    
    int loadAudioBuffer( int, int, std::string name);
  
  private:
    Top* top;
};


#endif
