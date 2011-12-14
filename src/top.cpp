
#include "top.hpp"

#include "controller.hpp"
#include "offlineworker.hpp"

using namespace std;

Top::Top() :
  state(this)
{
  cout << "Top()" << endl;
  
  controller = new Controller(this);
  offlineWorker = new OfflineWorker(this);
  guiDispatcher = new Glib::Dispatcher();
}
