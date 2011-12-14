
#include "g_statestore.hpp"

#include "glibmm.h"

using namespace std;

GuiStateStore::GuiStateStore()
{
  numTracks = 0;
  lastUsedDir = Glib::get_home_dir();
  cout << "Last used dir = " << lastUsedDir << endl;
  
  cutoff = 0.f;
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
  c->ID = -1;
  for ( int i = 0; i < 10; i++ )
    c->clipInfo.push_back( ClipInfo() );
  
  eqState.active = true;
  eqState.type = FREQUENCY_GRAPH_TYPE_PARAMETRIC;
  for(int i = 0; i < 4; i++)
  {
    eqState.Q[i] = 1.0;
    eqState.cutoffFreq[i] = (i/4.f) * 10000;
    eqState.gain[i] = 0.f;
  }
  
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

std::string GuiStateStore::getLastDir()
{
  return lastUsedDir;
}
void GuiStateStore::setLastDir(std::string s)
{
  lastUsedDir = s;
}

void GuiStateStore::setClipSelectorState(int t,int block, int bufferID)
{
  if ( t < clipSelectorState.size() ) {
    cout << "GuiStateStore::setClipSelectorState() track OOB " << t << endl;
    return;
  }
  
  if ( block < clipSelectorState.at(t).clipInfo.size() )
  {
    std::list<ClipInfo>::iterator iter = clipSelectorState.at(t).clipInfo.begin();
    std::advance(iter, block);
    
    (*iter).state = CLIP_STATE_LOADED;
  }
  else
  {
    cout << "GuiStateStore::setClipSelectorState(), block OOB" << block << endl;
  }
}
