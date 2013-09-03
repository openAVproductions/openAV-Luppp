
#include "saveable.hxx"

#include "../jack.hxx"
#include "save.hxx"

extern Jack* jack;

SaveAble::SaveAble()
{
  jack->getSave()->registerSaveable( this );
}

void SaveAble::save(){};
