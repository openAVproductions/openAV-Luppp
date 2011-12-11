
#include "statestore.hpp"

#include "top.hpp"

using namespace std;

StateStore::StateStore(Top* t)
{
  numTracks = 0;
  top = t;
}

void StateStore::addTrack()
{
  fileAudioSourceState.push_back( FileAudioSourceState() );
  FileAudioSourceState* i = &fileAudioSourceState.back();
  i->index = 0.f;
  i->speed = 1.f;
  i->bufferID = -1;
  
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
  
  numTracks++;
}

void StateStore::setAudioBuffer(AudioBuffer* bufPtr)
{
  cout << "StateStore::setAudioBuffer() Got AudioBuffer ID " << bufPtr->getID() << endl;
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
  
  EngineEvent* x = new EngineEvent();
  x->setMixerVolume(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
  
  //std::cout << "StateStore::setVolume() Track: " << t << ", linVol:" << v << "  logVol:" << logVolume << std::endl;
}

void StateStore::setPan(int t, float v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setPan() track OOB" << std::endl; return;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->pan = v;
  std::cout << "New pan: " << iter->pan << std::endl;
  
  EngineEvent* x = new EngineEvent();
  x->setTrackPan(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::setMute(int t, int v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setMute() track OOB" << std::endl; return;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->mute = v;
  
  EngineEvent* x = new EngineEvent();
  x->setTrackMute(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::setSolo(int t, int v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setSolo() track OOB" << std::endl; return;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->solo = v;
  
  EngineEvent* x = new EngineEvent();
  x->setTrackSolo(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::setRec(int t, int v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setRec() track OOB" << std::endl; return;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->rec = v;
  
  EngineEvent* x = new EngineEvent();
  x->setTrackRec(t, v);
  top->toGuiQueue.push(x);
  top->guiDispatcher->emit();
}

void StateStore::setPanZ(int t, float v)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::setPanZ() track OOB" << std::endl; return;
  }
  std::list<TrackOutputState>::iterator iter = trackoutputState.begin();
  std::advance(iter, t);
  iter->panZ = v;
}

void StateStore::setClipSelectorState(int t,int block, int bufferID)
{
  std::list<ClipSelectorState>::iterator iter = clipSelectorState.begin();
  std::advance(iter, t);
  
  if ( block < iter->clipInfo.size() )
  {
    iter->clipInfo.at(block).state = CLIP_STATE_LOADED;
  }
  else
  {
    cout << "StateStore::setClipSelectorState(), block OOB" << block << endl;
  }
}

TrackOutputState* StateStore::getAudioSinkOutput(int t)
{
  if ( !trackCheck(t) ) {
    std::cout << "StateStore::getAudioSinkOutput() track OOB: " << t << std::endl; return 0;
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
