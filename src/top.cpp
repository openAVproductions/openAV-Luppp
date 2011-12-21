
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

// this function is called from JackClient(), and serve the purpose of
// initializing variables *after* the bufferSize is known. Primary function
// is the scope view in the GUI / live waveform passing
void Top::initialize()
{
  scopeVector.resize(bufferSize);
}
