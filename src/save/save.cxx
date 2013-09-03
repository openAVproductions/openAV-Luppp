
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
  cout << " Save::save() " << endl;
  for( unsigned int i = 0; i < saveables.size(); i++)
  {
    saveables.at(i)->save(); 
  }
  
  // now each SaveAble has reqested save state, but not yet *saved* its state.
  // we need a way to determine if the saves are flushed.
}

void Save::finish()
{
  // trigger the GUI to write the metadata to disk, as each component of the
  // engine is done saving
  
  EventSaveFinish e;
  writeToGuiRingbuffer( &e );
  
}
