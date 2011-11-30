
#ifndef LUPPP_FILEAUDIOSOURCE
#define LUPPP_FILEAUDIOSOURCE

#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

using std::string;

class FileAudioSource : AudioSource
{
  public:
    FileAudioSource(string name);
    void process (int, float* );
    
    float index;
    float speed;
    std::vector<float> sampleBuffer;
};


#endif
