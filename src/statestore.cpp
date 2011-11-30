
#include "statestore.hpp"

StateStore::StateStore()
{
  
}

void StateStore::addTrack()
{
  FileAudioSourceState s;
  s.index = 0.f;
  s.speed = 1.f;
  
  fileAudioSourceState.push_back( s );
}
