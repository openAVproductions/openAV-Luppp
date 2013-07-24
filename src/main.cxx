

// Library
#include <iostream>
#include <unistd.h>

#include <jack/ringbuffer.h>

// Internal
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

int main()
{
  // setup the environment
  AVOIDDENORMALS();
  
  // allocate data to read from
  processDspMem = (char*)malloc( sizeof(EventBase) );
  processGuiMem = (char*)malloc( sizeof(EventBase) );
  
  rbToDsp = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  rbToGui = jack_ringbuffer_create( 5000 * sizeof(EventBase));
  
  
  //jack = new Jack();
  //jack->activate();
  
  gui = new Gui();
  gui->show();
  
}
