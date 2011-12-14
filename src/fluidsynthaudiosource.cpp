
#include "fluidsynthaudiosource.hpp"

using namespace std;

FluidSynthAudioSource::FluidSynthAudioSource(Top* t, std::string name)
{
  top = t;
  
  dummyOutput.resize( top->bufferSize );
  
  settings = new_fluid_settings();
  synth = new_fluid_synth(settings);
  
  std::string openFile = "example.sf2";
  
  std::cout << "FluidSynth()  Opening file : "<< openFile<< std::endl;
  
  // open file
  int soundfontID = fluid_synth_sfload(synth, openFile.c_str(), true); // true = reset patch
  
  fluid_synth_program_select( synth,
                              0,
                              soundfontID,
                              0,
                              0 );
  
  fluid_synth_get_channel_info(synth, 0, &channelInfo);
  
  std::cout << "FluidSynth::openFile() : bank 0, program 0 Name = " << channelInfo.name << std::endl;
  
  
  if ( soundfontID == FLUID_FAILED )
    std::cout << "ERROR LOADING SOUNDFONT FILE!" << std::endl;
}

void FluidSynthAudioSource::process (int nframes, float* buffer )
{
  /*
  // get playback variables
  std::list<FluidSynthAudioSourceState>::iterator iter = top->state.fileAudioSourceState.begin();
  std::advance(iter, ID);
  */
  
  float* dummy = &dummyOutput[0];
  fluid_synth_nwrite_float ( synth, nframes, &buffer, &dummy , 0, 0 );
  
  
}
