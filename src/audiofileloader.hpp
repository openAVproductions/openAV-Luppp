
#ifndef LUPPP_AUDIOFILELOADER
#define LUPPP_AUDIOFILELOADER

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

class Top;

#include "audiobuffer.hpp"

class AudioFileLoader
{
  public:
    AudioFileLoader(Top*);
    
    int load( int, int, std::string name);
  
  private:
    Top* top;
};


#endif
