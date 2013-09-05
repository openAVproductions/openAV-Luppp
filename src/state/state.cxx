
#include "state.hxx"

#include <iostream>
#include "../event.hxx"
#include "../eventhandler.hxx"

using namespace std;

State::State()
{
}

void State::registerStately(Stately* s)
{
  //cout << "State::registerStately() " << s << endl;
  statelys.push_back( s );
}

void State::save()
{
#ifdef DEBUG_STATE
  cout << " State::save() " << endl;
#endif
  for( unsigned int i = 0; i < statelys.size(); i++)
  {
    statelys.at(i)->save(); 
  }
}

void State::reset()
{
#ifdef DEBUG_STATE
  cout << " State::reset() " << endl;
#endif
  for( unsigned int i = 0; i < statelys.size(); i++)
  {
    statelys.at(i)->reset(); 
  }
}

void State::finish()
{
  // trigger the GUI to write the metadata to disk, as each component of the
  // engine is done saving
  
  EventStateSaveFinish e;
  writeToGuiRingbuffer( &e );
  
}

int State::getNumStatelys()
{
  return statelys.size();
}
