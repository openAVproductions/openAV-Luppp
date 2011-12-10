
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
