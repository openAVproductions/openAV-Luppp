
#include "state.hxx"

#include <iostream>
#include "../event.hxx"
#include "../eventhandler.hxx"

#include "../jack.hxx"

extern Jack* jack;

using namespace std;

State::State()
{
}

void State::registerStately(Stately* s)
{
  statelys.push_back( s );
}

void State::save()
{
  for( unsigned int i = 0; i < statelys.size(); i++)
  {
    statelys.at(i)->save(); 
  }
}

void State::reset()
{
  for( unsigned int i = 0; i < statelys.size(); i++)
  {
    statelys.at(i)->reset(); 
  }
  
  jack->getGridLogic()->updateState();
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
