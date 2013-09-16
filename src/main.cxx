
#include "config.hxx"

#include <iostream>
#include <unistd.h>

#include <jack/ringbuffer.h>

#include "gui.hxx"
#include "jack.hxx"
#include "event.hxx"
#include "denormals.hxx"

char* processDspMem = 0;
char* processGuiMem = 0;

jack_ringbuffer_t* rbToDsp = 0;
jack_ringbuffer_t* rbToGui = 0;

// global static pointers, for access from EventHandlerGui and EventHandlerDsp
Gui * gui  = 0;
Jack* jack = 0;


int main(int argc, char** argv)
{
  // setup the environment
  AVOIDDENORMALS();
  
  // allocate data to read from
  processDspMem = (char*)malloc( sizeof(EventBase) );
  processGuiMem = (char*)malloc( sizeof(EventBase) );
  
  rbToDsp = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  rbToGui = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  
  gui = new Gui();
  jack = new Jack();
  
#ifdef BUILD_TESTS
  // test offline functionality
  gui->getDiskWriter()->runTests();
  // test realtime functionality
  jack->getGridLogic()->runTests();
  
#ifdef BUILD_COVERAGE_TEST
  LUPPP_NOTE("%s","Done testing, quitting!");
  return 0;
#endif
  // FIXME: Reset the state of GUI / GridLogic here. Create a "new session"?
#endif
  cout << "Done testing... launching Luppp." << endl;
  jack->activate();
  gui->show();
}

