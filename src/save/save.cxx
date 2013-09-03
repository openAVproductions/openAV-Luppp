
#include "save.hxx"

Save::Save()
{
  
}

void Save::registerSaveable(SaveAble* s)
{
  saveables.push_back( s );
}

void Save::save()
{
  for( unsigned int i = 0; i < saveables.size(); i++)
  {
    saveables.at(i)->save(); 
  }
}
