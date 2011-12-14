
#include "top.hpp"

#include "controller.hpp"

using namespace std;

Top::Top() :
  state(this),
  audioFileLoader(this)
{
  cout << "Top()" << endl;
  
  controller = new Controller(this);
  
  guiDispatcher = new Glib::Dispatcher();
}
