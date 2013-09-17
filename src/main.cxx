
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
  bool runTests = false;
  bool stopAfterTest = false;
  for(int i = 0; i < argc; i++)
  {
    if ( strcmp(argv[i], "-test" ) == 0 ) {
      runTests = true;
    } else if ( strcmp( argv[i], "-stopAfterTest") == 0 ) {
      stopAfterTest = true;
    }
  }
  
  // setup the environment
  AVOIDDENORMALS();
  
  // allocate data to read from
  processDspMem = (char*)malloc( sizeof(EventBase) );
  processGuiMem = (char*)malloc( sizeof(EventBase) );
  
  rbToDsp = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  rbToGui = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  
#ifdef BUILD_TESTS
  if ( runTests )
  {
    // setup the testing Gui / JACK
    gui = new Gui();
    jack = new Jack();
    
    // test offline functionality
    gui->getDiskWriter()->runTests();
    
    
    delete gui;
    delete jack;
    
    gui = new Gui();
    jack = new Jack();
    
    // test realtime functionality
    jack->getGridLogic()->runTests();
    
    delete gui;
    delete jack;
    
#ifdef BUILD_COVERAGE_TEST
    if ( stopAfterTest )
    {
      LUPPP_NOTE("%s","Done testing, quitting!");
      return 0;
    }
#endif
  }
  // FIXME: Reset the state of GUI / GridLogic here. Create a "new session"?
#endif
  
  // setup the "real" GUI / JACK
  gui = new Gui();
  jack = new Jack();
  
  jack->activate();
  gui->show();
}

