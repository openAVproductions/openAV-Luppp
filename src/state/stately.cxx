
#include "stately.hxx"

#include "../jack.hxx"
#include "state.hxx"

extern Jack* jack;

int Stately::saveSuccess = 0;
int Stately::saveErrors  = 0;

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

void Stately::checkCompletedSave()
{
  if ( (saveSuccess + saveErrors) >= jack->getState()->getNumStatelys() )
  {
    jack->getState()->finish();
    
    // reset in case of another save before quit
    saveErrors  = 0;
    saveSuccess = 0;
  }
}

void Stately::success()
{
  saveSuccess++;
  checkCompletedSave();
}

void Stately::error(const char* errorString)
{
  // CRITICAL FIXME: add error handling code, noting an error occured, perhaps prompt user?
  saveErrors++;
  checkCompletedSave();
}
