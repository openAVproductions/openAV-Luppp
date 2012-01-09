
#ifndef LUPPP_WHITENOISEAUDIOSOURCE
#define LUPPP_WHITENOISEAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>

#include "audiosource.hpp"

class Top;

using std::string;

class WhiteNoiseAudioSource : public AudioSource
{
  public:
    WhiteNoiseAudioSource(Top*);
    
    void process (int, float* );
  
  private:
    Top* top;
    int ID;
    
    std::vector<float> tmpOutput;
    std::vector<float> dummyOutput;
};


#endif
