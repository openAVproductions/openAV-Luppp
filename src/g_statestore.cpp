
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
  
  masterClipPlaying = -1;
  
  lastUsedDir = Glib::get_home_dir();
  cout << "Last used dir = " << lastUsedDir << endl;
}

AudioBuffer* GuiStateStore::getNewAudioBuffer()
{
  cout << "GuiStateStore::getNewAudioBuffer() called!" << endl;
  
  // create an AudioBuffer object that's shared, and keep push it to the vector
  audioBufferVector.push_back( std::make_shared<AudioBuffer>() );
  
  // return the new AudioBuffer* (raw pointer, needs to be a shared_ptr eventually)
  return audioBufferVector.back().get();
}

AudioBuffer* GuiStateStore::getAudioBufferPointer(int ID)
{
  if ( ID >= 0 && ID < audioBufferVector.size() )
  {
    // return the new AudioBuffer* (raw pointer, needs to be a shared_ptr eventually)
    return audioBufferVector.at(ID).get();
  }
  
  return 0;
}

WaveformCache* GuiStateStore::getNewWaveformCache()
{
  waveformCacheVector.push_back( std::make_shared<WaveformCache>() );
  
  return waveformCacheVector.back().get();
}

WaveformCache* GuiStateStore::getWaveformCache(int ID)
{
  // check if we have a generated waveform of the buffer here
  for ( int i = 0; i < waveformCacheVector.size(); i++ )
  {
    if ( ID == waveformCacheVector.at(i)->getID() )
    {
      return waveformCacheVector.at(i).get();
    }
  }
  
  return 0;
}

void GuiStateStore::addTrack()
{
  //std::cout << "GuiStateStore::addTrack()" << std::endl;
  
  // don't store ID for the vector, its indice is the ID, so just access
  // the array .at(ID) rather than iter over it, and take ( targetID == ID )
  effectState.push_back ( EffectState(-1) );
  
  bufferAudioSourceState.push_back( BufferAudioSourceState() );
  BufferAudioSourceState* i = &bufferAudioSourceState.back();
  
  i->index = -1.f; // sets waveview playhead off screen
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
  
  audioBufferNameVector.push_back( BlockIdName() );
  audioBufferNameVector.back().ID = ID;
  audioBufferNameVector.back().name = name;
  
  for ( int i = 0; i < audioBufferNameVector.size(); i++)
  {
    cout << i << " " << audioBufferNameVector.at(i).ID << "  " << audioBufferNameVector.at(i).name << endl;
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
