
#ifndef LUPPP_FILEAUDIOSOURCE
#define LUPPP_FILEAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

//class Top;

struct FileAudioSourceState
{
  float speed, index;
};

using std::string;

class FileAudioSource : public AudioSource
{
  public:
    FileAudioSource(string name);
    
    void setParameter(int p,float s);
    void process (int, float* );
    
    float index;
    float speed;
    
    std::vector<float> sampleBuffer;
  private:
    //Top* top;
};


#endif
