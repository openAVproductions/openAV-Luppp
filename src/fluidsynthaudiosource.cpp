
#include "fluidsynthaudiosource.hpp"

using namespace std;

FluidSynthAudioSource::FluidSynthAudioSource(Top* t, std::string name)
{
  top = t;
  
  ID = AudioSource::getID();
  
  tmpOutput.resize( top->bufferSize );
  dummyOutput.resize( top->bufferSize );
  
  settings = new_fluid_settings();
  synth = new_fluid_synth(settings);
  
  std::string openFile = "example.sf2";
  
  std::cout << "FluidSynth() ID = " << ID << "  Opening file : "<< openFile<< std::endl;
  
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
  
  if ( top->state.midib1 != -1 )
  {
    std::cout << "FluidSynth MIDI event: " << top->state.midib1 << ", " << top->state.midib2 << ", " << top->state.midib1 << std::endl;
    
    switch ( top->state.midib1 )
    {
      case 144: fluid_synth_noteon (synth, 0, top->state.midib2 , top->state.midib3); break;
      case 128: fluid_synth_noteoff(synth, 0, top->state.midib2 ); break;
    }
  }
  
  float* tmp = &dummyOutput[0];
  float* dummy = &dummyOutput[0];
  fluid_synth_nwrite_float ( synth, nframes, &tmp, &dummy , 0, 0 );
  
  for ( int i = 0; i < nframes; i++ )
  {
    *buffer++ += *tmp++;
  }
}
