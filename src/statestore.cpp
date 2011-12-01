
#include "statestore.hpp"

using namespace std;

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
  
  cout << "StateStore::addTrack()  index , speed = " << i->index << ", " << i->speed << endl;
  
}
