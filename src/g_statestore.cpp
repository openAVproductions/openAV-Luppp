
#include "g_statestore.hpp"

using namespace std;

GuiStateStore::GuiStateStore()
{
  numTracks = 0;
}

void GuiStateStore::addTrack()
{
  std::cout << "GuiStateStore::addTrack()" << std::endl;
  
  fileAudioSourceState.push_back( FileAudioSourceState() );
  FileAudioSourceState* i = &fileAudioSourceState.back();
  i->index = 0.f;
  i->speed = 1.f;
  
  clipSelectorState.push_back( ClipSelectorState() );
  ClipSelectorState* c = &clipSelectorState.back();
  c->ID = 1;
  
  
  
  trackoutputState.push_back( TrackOutputState() );
  trackoutputState.back().ID = numTracks;
  trackoutputState.back().selected = false;
  trackoutputState.back().volume = 0.f;
  trackoutputState.back().pan = 0.f;
  
  if ( numTracks == 1 )
    trackoutputState.back().selected = true;
  
  
  cout << "GuiStateStore::addTrack()  index , speed = " << i->index << ", " << i->speed << endl;
  
  numTracks++;
}
