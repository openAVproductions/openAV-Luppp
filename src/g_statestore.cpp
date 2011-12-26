
#include "g_statestore.hpp"

#include <glibmm.h>

using namespace std;

GuiStateStore::GuiStateStore()
{
  numTracks = 0;
  lastUsedDir = Glib::get_home_dir();
  cout << "Last used dir = " << lastUsedDir << endl;
}

void GuiStateStore::addTrack()
{
  //std::cout << "GuiStateStore::addTrack()" << std::endl;
  
  // don't store ID for the vector, its indice is the ID, so just access
  // the array .at(ID) rather than iter over it, and take ( targetID == ID )
  effectState.push_back ( EffectState(-1) );
  
  bufferAudioSourceState.push_back( BufferAudioSourceState() );
  BufferAudioSourceState* i = &bufferAudioSourceState.back();
  i->index = 0.f;
  i->speed = 1.f;
  
  clipSelectorState.push_back( ClipSelectorState() );
  ClipSelectorState* c = &clipSelectorState.back();
  c->ID = -1;
  for ( int i = 0; i < 10; i++ )
    c->clipInfo.push_back( ClipInfo() );
  
  trackoutputState.push_back( TrackOutputState() );
  trackoutputState.back().ID = numTracks;
  trackoutputState.back().selected = false;
  trackoutputState.back().selectedDevice = 0;
  trackoutputState.back().volume = 0.f;
  trackoutputState.back().pan = 0.f;
  
  if ( numTracks == 0 )
  {
    trackoutputState.back().selected = true;
  }
  
  //cout << "GuiStateStore::addTrack()  index , speed = " << i->index << ", " << i->speed << endl;
  
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
    
    //(*iter).state = CLIP_STATE_LOADED;
  }
  else
  {
    cout << "GuiStateStore::setClipSelectorState(), block OOB" << block << endl;
  }
}
