
#ifndef LUPPP_FILEAUDIOSOURCE
#define LUPPP_FILEAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

#include "bufferaudiosourcestate.hpp"

#include "top.hpp"

using std::string;

class BufferAudioSource : public AudioSource
{
  public:
    BufferAudioSource(Top*);
    void process (int, float* );
  
  private:
    Top* top;
    int ID;
};


#endif
