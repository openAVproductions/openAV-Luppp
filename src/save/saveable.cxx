
#include "saveable.hxx"

#include "../jack.hxx"
#include "save.hxx"

extern Jack* jack;

int SaveAble::savesDone = 0;

SaveAble::SaveAble()
{
  jack->getSave()->registerSaveable( this );
}


void SaveAble::reset()
{
}

void SaveAble::save()
{
}

void SaveAble::done()
{
  savesDone++;
  
  if ( savesDone >= jack->getSave()->getNumSaveables() )
  {
    jack->getSave()->finish();
    savesDone = 0; // reset in case of another save before quit
  }
}
