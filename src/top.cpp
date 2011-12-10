
#include "top.hpp"

using namespace std;

Top::Top() :
  state(this)
{
  cout << "Top()" << endl;
  
  guiDispatcher = new Glib::Dispatcher();
  
  addTrack();
  addTrack();
}

void Top::addTrack()
{
  
}
