
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

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

void GuiStateStore::addAudioBufferName(int ID, std::string name)
{
  
  audioBufferNameVector.push_back( name );
  
  for ( int i = 0; i < audioBufferNameVector.size(); i++)
  {
    cout << i << " " << audioBufferNameVector.at(i) << endl;
  }
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
