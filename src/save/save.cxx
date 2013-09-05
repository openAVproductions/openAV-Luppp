
#include "save.hxx"

#include <iostream>
#include "../event.hxx"
#include "../eventhandler.hxx"

using namespace std;

Save::Save()
{
  
}

void Save::registerSaveable(SaveAble* s)
{
  //cout << "Save::registerSaveable() " << s << endl;
  saveables.push_back( s );
}

void Save::save()
{
#ifdef DEBUG_SAVE
  cout << " Save::save() " << endl;
#endif
  for( unsigned int i = 0; i < saveables.size(); i++)
  {
    saveables.at(i)->save(); 
  }
}

void Save::reset()
{
#ifdef DEBUG_SAVE
  cout << " Save::reset() " << endl;
#endif
  for( unsigned int i = 0; i < saveables.size(); i++)
  {
    saveables.at(i)->reset(); 
  }
}

void Save::finish()
{
  // trigger the GUI to write the metadata to disk, as each component of the
  // engine is done saving
  
  EventSaveFinish e;
  writeToGuiRingbuffer( &e );
  
}
