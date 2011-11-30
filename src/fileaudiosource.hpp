
#ifndef LUPPP_FILEAUDIOSOURCE
#define LUPPP_FILEAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

struct FileAudioSourceState
{
  float speed, index;
};

using std::string;

class FileAudioSource : AudioSource
{
  public:
    FileAudioSource(Top*, string name);
    void process (int, float* );
    
    std::vector<float> sampleBuffer;
  private:
    Top* top;
};


#endif
