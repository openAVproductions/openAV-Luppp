
#include "config.hxx"

#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <jack/ringbuffer.h>

#include "gui.hxx"
#include "jack.hxx"
#include "event.hxx"
#include "denormals.hxx"

int signalHanlderInt = 0;

char* processDspMem = 0;
char* processGuiMem = 0;

jack_ringbuffer_t* rbToDsp = 0;
jack_ringbuffer_t* rbToGui = 0;

// global static pointers, for access from EventHandlerGui and EventHandlerDsp
Gui * gui  = 0;
Jack* jack = 0;


void signalHanlder(int signum)
{
  signalHanlderInt = signum;
}

int main(int argc, char** argv)
{
  bool runTests = false;
  bool stopAfterTest = false;
  if(runTests == stopAfterTest){} // warning
  
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
  signal(SIGINT , signalHanlder);
  signal(SIGTERM, signalHanlder);
  
  // allocate data to read from
  processDspMem = (char*)malloc( sizeof(EventBase) );
  processGuiMem = (char*)malloc( sizeof(EventBase) );
  
  rbToDsp = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  rbToGui = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  
  /*
#ifdef BUILD_TESTS
  if ( runTests )
  {
    // counts failures
    int testResult = 0;
    
    // setup the testing Gui / JACK: Jack first, then GUI
    Jack::setup();
    //jack = new Jack();
    gui = new Gui(argv[0]);
    
    // test offline functionality
    testResult += gui->getDiskReader()->runTests();
    testResult += gui->getDiskWriter()->runTests();
    
    // test realtime functionality
    testResult += jack->getGridLogic()->runTests();
    
    delete gui;
    delete jack;
    
#ifdef BUILD_COVERAGE_TEST
    if ( stopAfterTest )
    {
      return testResult;
    }
#endif
  }
#endif
  */
  
  // setup the "real" JACK / Gui: Jack first, then GUI
  gui = new Gui(argv[0]);
  
  if ( gui->getNsm() )
  {
    // the NSM OSC Open message will trigger Jack initialization: necessary
    // to use the right name to create the JACK client.
  }
  else
  {
    Jack::setup("Luppp");
    jack->activate();
  }
  
  gui->show();
  
  return 0;
}

