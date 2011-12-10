
#include "statestore.hpp"

using namespace std;

StateStore::StateStore()
{
  numTracks = 0;
}

void StateStore::addTrack()
{
  fileAudioSourceState.push_back( FileAudioSourceState() );
  FileAudioSourceState* i = &fileAudioSourceState.back();
  i->index = 0.f;
  i->speed = 1.f;
  
  clipSelectorState.push_back( ClipSelectorState() );
  ClipSelectorState* c = &clipSelectorState.back();
  c->ID = 1;
  
  trackoutputState.push_back( TrackOutputState() );
  std::list<TrackOutputState>::iterator outputStateIter = trackoutputState.begin();
  std::advance(outputStateIter, numTracks);
  outputStateIter->ID = numTracks;
  outputStateIter->selected = false;
  outputStateIter->volume = 0.f;
  outputStateIter->pan = 0.f;
  
  if ( numTracks == 1 )
    outputStateIter->selected = true;
  
  numTracks++;
}

void StateStore::setVolume(int t, float v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setVolume() track OOB" << std::endl; return;
  }
  
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  
  // Clip vol input to range
  if (v > 1.0)
    v = 1.0;
  else if (v < 0.00001)
    v = 0.0;
  
  // linear input, hence we need to convert the [0,1] range
  // to a logarithmic scale. Formula used:
  // y = x ^ 3, this is an approximation of the human hearing curve.
  // http://www.dr-lex.be/info-stuff/logVolumecontrols.html for info
  float volMultiplier = (float)pow( v , 3);
  
  // apply scaling to volMultiplier, so we can amplify to +50%
  float logVolume = volMultiplier * 2;
  
  iter->volume = logVolume;
  
  //std::cout << "StateStore::setVolume() Track: " << t << ", linVol:" << v << "  logVol:" << logVolume << std::endl;
}

TrackOutputState* StateStore::getAudioSinkOutput(int t)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::getAudioSinkOutput() track OOB" << std::endl; return 0;
  }
  
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  
  return &(*iter);
}

bool StateStore::trackCheck(int t)
{
  if ( t >= numTracks )
    return false;
  
  return true;
}
