
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

#include "statestore.hpp"

#include "top.hpp"

#include "jackclient.hpp"
#include "effectstate.hpp"

using namespace std;

StateStore::StateStore(Top* t)
{
  numTracks = 0;
  top = t;
  
  queueClips = false;
  
  globalUnit = 0.f;
  globalUnitType = AUTOMOVE_TYPE_NONE;
  
}

void StateStore::addEffect(int t, int pos, Effect* effect)
{
  // here we are passed a pointer to the desired effect,
  // and told what track & position to insert it
}

void StateStore::addTrack(BufferAudioSourceState* bas, ClipSelectorState* css, TrackOutputState* tos)
{
  /*
  cout << "StateStore::addTrack() BuffAudioSource* = " << bas
                              << "  ClipSelectorState "<< css
                              << "  TrackOutputState " << tos << endl;
  */
  
  bufferAudioSourceState.push_back( *bas );
  
  // initialize the clipSelectorState for this track
  clipSelectorState.push_back( ClipSelectorState() );
  ClipSelectorState* c = &clipSelectorState.back();
  c->ID = -1;
  for ( int i = 0; i < 10; i++ )
  {
    c->clipInfo.push_back( ClipInfo() );
  }
  
  // initialize the TrackOutputState
  trackoutputState.push_back( TrackOutputState() );
  std::list<TrackOutputState>::iterator outputStateIter = trackoutputState.begin();
  std::advance(outputStateIter, numTracks);
  outputStateIter->ID = numTracks;
  outputStateIter->selected = false;
  outputStateIter->volume = 0.f;
  outputStateIter->pan = 0.f;
  outputStateIter->panZ = 0.f;
  
  top->jackClient->writeMidi( top->jackClient->getApcOutputBuffer(), 144 + numTracks, 50, 127 ); // mute LED on
  top->jackClient->writeMidi( top->jackClient->getApcOutputBuffer(), 144 + numTracks, 52, 127 ); // Clip Stop LED on
  
  numTracks++;
}

void StateStore::setAudioBuffer(AudioBuffer* bufPtr)
{
  //cout << "StateStore::setAudioBuffer() Got AudioBuffer ID " << bufPtr->getID() << endl;
  audiobufferList.push_back( *bufPtr );
}

AudioBuffer* StateStore::getAudioBuffer(int ID)
{
  if ( ID == -1 ) // -1 means not loaded, save itering the list :)
    return 0;
  
  std::list<AudioBuffer>::iterator iter;
  
  for ( iter = audiobufferList.begin(); iter != audiobufferList.end(); iter++ )
  {
    if ( iter->getID() == ID )
    {
      return &(*iter);
    }
  }
  
  return 0;
}

void StateStore::addEffectState(EffectState* newEffectState)
{
  //cout << "StateStore::addEffectState()  ID = " << newEffectState->ID << endl;
  // store the new ID in the EffectState, so it can be retrieved later
  effectStateList.push_back( newEffectState );
}

// this function takes the UniqueID of the Effect, and returns the corresponding
// EffectState as a pointer. This way we can shuffle the position of Effects,
// without having to update the stateStore in the same way (more effort / bugs)
EffectState* StateStore::getEffectState(int ID)
{
  std::list<EffectState*>::iterator iter;
  for ( iter = effectStateList.begin(); iter != effectStateList.end(); iter++ )
  {
    if ( (*iter)->ID == ID )
    {
      return &(*(*iter));
    }
  }
  
  return 0;
}

int StateStore::setVolume(int t, float v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setVolume() track OOB" << std::endl; return -1;
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
  
  // here we can apply scaling if we want to amplify the signal, not only
  // attenuate it. This may be a useful feature for quiet recordings, but
  // its also quite irritating when using a control surface where the faders
  // move swift & we just want the "full" volume, no amplification.
  float logVolume = volMultiplier;
  
  iter->volume = logVolume;
  
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setMixerVolume(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  
  return 0;
  //std::cout << "StateStore::setVolume() Track: " << t << ", linVol:" << v << "  logVol:" << logVolume << std::endl;
}

int StateStore::setSend(int t, int send, float value)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setSend() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->sends = value;
  //std::cout << "New send level: " << iter->sends << "  on track " << t << std::endl;
  
  // to GUI
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setTrackSend(t, send, value);
  top->toGuiQueue.push(x);
  
  return 0;
}

int StateStore::setPan(int t, float v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setPan() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->pan = v;
  //std::cout << "New pan: " << iter->pan << std::endl;
  
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setTrackPan(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  return 0;
}

int StateStore::setMute(int t, int v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setMute() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->mute = v;
  
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setTrackMute(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  
  top->jackClient->apcWriteOutput(t);
  
  return 0;
}

int StateStore::setSolo(int t, int v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setSolo() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->pflEnable = v;
  
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setTrackSolo(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  
  top->jackClient->apcWriteOutput(t);
  
  return 0;
}

int StateStore::setRec(int t, int v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setRec() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->recEnable = v;
  //cout << "StateStore::setRec()  Track " << t << " has REC ENABLE = " << v << endl;
  
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->setTrackRec(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  
  top->jackClient->apcWriteOutput(t);
  
  return 0;
}

int StateStore::setPanZ(int t, float v)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::setPanZ() track OOB" << std::endl; return -1;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->panZ = v;
  return 0;
}

void StateStore::setClipSelectorState(int t,int block, int bufferID)
{
  std::list<ClipSelectorState>::iterator iter = clipSelectorState.begin();
  std::advance(iter, t);

  std::list<ClipInfo>::iterator clipI = iter->clipInfo.begin();
  std::advance(clipI, block);
  
  clipI->bufferID = bufferID;
  clipI->hasBuffer= true;
  
  // update APC
  top->jackClient->apcWriteGridTrack(t);
  
  // bufferID here is used to extract name from GUI list of AudioBuffer names
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->looperLoad(t, block, bufferID);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::clipSelectorQueueClip(int t, int b)
{
  // marks a clip as queued, and redraws controllers
  std::list<ClipSelectorState>::iterator iter = clipSelectorState.begin();
  std::advance(iter, t);
  
  //cout << "clipSelectorQueueCLip() T = " << t << ", block = " << b << endl;
  
  iter->queued = b;
}

// this function handles the logic in activating / deactivating and otherwise
// marking every Clip in Engine. It will test if RecordEnable is active on the
// track, and if it is, will record into a pressed buffer, otherwise it will
// play it back.
void StateStore::clipSelectorActivateClip(int t, int b)
{
  //cout << " clipSElectorActivateClip() track: " << t << "   block " << b << endl;
  // we get a track & scene number, so we set them in the ClipSelectorState
  // later the playback will request the bufferID ClipInfo of the right position in the list
  //std::cout << "StateStore::clipSelectorActivateClip() " << t << ", " << b << endl;
  std::list<ClipSelectorState>::iterator iter = clipSelectorState.begin();
  std::advance(iter, t);
  
  TrackOutputState* trackState = getAudioSinkOutput(t);
  if ( trackState == 0 ) { return; } // grid press on non existent track
  
  if ( b >= 0 ) // b != stop clip (-1) so safe to access
  {
    if ( trackState->recEnable && top->jackClient->recordInput == false )
    {
      //cout << "Clip @ " << t << "  block " << b << " pressed while REC ENABLE, starting RECORDING NOW!" << endl;
      top->jackClient->recordInput = true;
      iter->recording = b;
      
      // event to GUI to start recording
      EngineEvent* x = top->toEngineEmptyEventQueue.pull();
      x->setLooperRecord(t, b, true);
      top->toGuiQueue.push(x);
    }
    else
    {
      //cout << "Clip @ " << t << "  block " << b << " block pressed while recording on this track!" << endl;
      
      if ( iter->recording == b )
      {
        //cout << "RECORDING BLOCK PRESSED! \t\t\t\tSTOP RECORD & SEND GUI!" << endl;
        // this Clip has been recording ( other clips might be pressed inbetween
        // record & stop-record presses) so we tell JACK to stop recording, and
        // tell the GUI thread that we want the recorded buffer loaded to the current
        // button released block.
        top->jackClient->recordInput = false; // JACK stops recording
        
        iter->recording = -1;
        
        // event to GUI to stop recording
        EngineEvent* x = top->toEngineEmptyEventQueue.pull();
        x->setLooperRecord(t, b, false);
        top->toGuiQueue.push(x);
      }
      else
      {
        //cout << "its not the clip that *IS* recording!" << endl;
        // set the "Playback" variables here, as we DON'T want to change the
        // currently playing buffer when we have the track in Record mode!
        std::list<BufferAudioSourceState>::iterator iterBASS = bufferAudioSourceState.begin();
        std::advance(iterBASS, t);
        iterBASS->index = 0; // restart sample from beginning
      }
      
    }
  
  } // b > 0
  else
  {
    iter->playing = -1;
    iter->queued  = -1;
  }
  
  iter->playing = b; // write current playing regardless of what block it is
  
  // update APC grid
  top->jackClient->apcWriteGridTrack(t);
  
  // update GUI
  EngineEvent* x = top->toEngineEmptyEventQueue.pull();
  x->looperSelectBuffer(t, b);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::setPluginActive(int UID, int active)
{
  std::list<EffectState*>::iterator iter;
  for ( iter = effectStateList.begin(); iter != effectStateList.end(); iter++ )
  {
    if ( (*iter)->ID == UID )
    {
      //cout << "StateStore::setPluginActive() " << UID << "  " << active << endl;
      (*iter)->active = active;
      
      EngineEvent* x = top->toEngineEmptyEventQueue.pull();
      x->setTrackDeviceActive(UID, active);
      top->toGuiQueue.push(x);
      
      break;
    }
  }
}

void StateStore::setPluginParameter(int ID, int param, float value)
{
  // all "dynamic" elements in engine ( ladspaHosts, lv2host, etc ) all use
  // the same generic "EffectState" as thier settings. Up to 8 floats of control
  // for now, more is *not* suitable for RT LIVE performance.
  
  //cout << "StateStore::setPluginParameter() " << t << ", " << pos << ", " << param << ", " << value << endl;
  
  // iter over the list, and 
  std::list<EffectState*>::iterator iter;
  for ( iter = effectStateList.begin(); iter != effectStateList.end(); iter++ )
  {
    if ( (*iter)->ID == ID )
    {
      break;
    }
  }
  
  if ( iter == effectStateList.end() )
  {
    //std::cout << "StateStore::setPluginPara() Error, did not find EffectState with ID " << ID << std::endl;
    return;
  }
  else
  {
    //std::cout << "StateStore::setPluginParam() writing value " << value << " to param " << param << endl;
    if ( value > 1.f ) value = 1.f;
    if ( value < 0.f ) value = 0.f;
    
    (*iter)->values[param] = value;
    
    // bounce on to GUI (-1 not used)
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setPluginParameter(ID, -1, param, value);
    top->toGuiQueue.push(x);
  }
}

void StateStore::setPluginGlobalUnit(int ID, int onOff)
{
  // all "dynamic" elements in engine ( ladspaHosts, lv2host, etc ) all use
  // the same generic "EffectState" as thier settings. Up to 8 floats of control
  // for now, more is *not* suitable for RT LIVE performance.
  
  //cout << "StateStore::setPluginGlobalUnit() " << t << ", " << param << ", " << onOff << endl;
  
  // iter over the list, and 
  std::list<EffectState*>::iterator iter;
  for ( iter = effectStateList.begin(); iter != effectStateList.end(); iter++ )
  {
    if ( (*iter)->ID == ID )
    {
      break;
    }
  }
  
  if ( iter == effectStateList.end() )
  {
    //std::cout << "StateStore::setPluginGlobalUnit() Error, did not find EffectState with ID " << ID << std::endl;
    return;
  }
  else
  {
    //std::cout << "StateStore::setPluginGlobalUnit() writing value " << onOff << endl;
    (*iter)->globalUnit = (int)onOff;
    
    // bounce on to GUI (-1 not used)
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setPluginGlobalUnit(ID, onOff);
    top->toGuiQueue.push(x);
  }
}

TrackOutputState* StateStore::getAudioSinkOutput(int t)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::getAudioSinkOutput() track OOB: " << t << std::endl;
    return 0;
  }
  
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  
  return &(*iter);
}

ClipSelectorState* StateStore::getClipSelectorState(int t)
{
  if ( !trackCheck(t) ) {
    //std::cout << "StateStore::getClipSelectorState() track OOB: " << t << std::endl;
    return 0;
  }
  
  std::list<ClipSelectorState>::iterator iter = clipSelectorState.begin();
  std::advance(iter, t);
  
  return &(*iter);
}

// convinience functions
bool StateStore::trackCheck(int t)
{
  if ( t >= numTracks && t >= 0 )
    return false;
  
  return true;
}

bool StateStore::effectCheck(int t)
{
  if ( t >= numEffects && t >= 0 )
    return false;
  
  return true;
}
