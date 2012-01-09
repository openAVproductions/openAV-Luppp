
#include "whitenoiseaudiosource.hpp"

#include "top.hpp"

#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;

WhiteNoiseAudioSource::WhiteNoiseAudioSource(Top* t)
{
  top = t;
  
  srand( (unsigned)time(0) );
  
  ID = AudioSource::getID();
  
  tmpOutput.resize( top->bufferSize );
  dummyOutput.resize( top->bufferSize );
  
}

void WhiteNoiseAudioSource::process (int nframes, float* buffer )
{
  /*
  // get playback variables
  std::list<WhiteNoiseAudioSourceState>::iterator iter = top->state.fileAudioSourceState.begin();
  std::advance(iter, ID);
  */
  
  for ( int i = 0; i < nframes; i++ )
  {
    *buffer++ += ((float)rand()) / RAND_MAX;
  }
}
