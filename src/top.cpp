
#include "top.hpp"

using namespace std;

Top::Top() :
  state(this),
  audioFileLoader(this)
{
  cout << "Top()" << endl;
  
  guiDispatcher = new Glib::Dispatcher();
}
