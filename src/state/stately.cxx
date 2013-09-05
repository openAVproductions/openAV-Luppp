
#include "stately.hxx"

#include "../jack.hxx"
#include "state.hxx"

extern Jack* jack;

int Stately::savesDone = 0;

Stately::Stately()
{
  jack->getState()->registerStately( this );
}


void Stately::reset()
{
}

void Stately::save()
{
}

void Stately::done()
{
  savesDone++;
  
  if ( savesDone >= jack->getState()->getNumStatelys() )
  {
    jack->getState()->finish();
    savesDone = 0; // reset in case of another save before quit
  }
}
