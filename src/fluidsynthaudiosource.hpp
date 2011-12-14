
#ifndef LUPPP_FLUIDSYNTHAUDIOSOURCE
#define LUPPP_FLUIDSYNTHAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>

#include <fluidsynth.h>

#include "audiosource.hpp"

#include "top.hpp"

using std::string;

class FluidSynthAudioSource : public AudioSource
{
  public:
    FluidSynthAudioSource(Top*,string name);
    
    void process (int, float* );
  
  private:
    Top* top;
    
    
    fluid_synth_t*    synth;
    fluid_settings_t* settings;
    fluid_synth_channel_info_t channelInfo;
    
    std::vector<float> tmpOutput;
    std::vector<float> dummyOutput;
};


#endif
