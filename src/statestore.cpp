
#include "statestore.hpp"

StateStore::StateStore()
{
  
}

void StateStore::addTrack()
{
  std::cout << "StateStore::addTrack()" << std::endl;
  
  fileAudioSourceState.push_back( FileAudioSourceState() );
  
  std::list<FileAudioSourceState>::iterator i = fileAudioSourceState.end();
  i->index = 0.f;
  i->speed = 1.f;
  
}
