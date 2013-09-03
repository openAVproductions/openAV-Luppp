
#include "save.hxx"

#include <iostream>

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
}
