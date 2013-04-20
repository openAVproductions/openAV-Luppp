

// Library
#include <iostream>
#include <unistd.h>

#include <jack/ringbuffer.h>

// Internal
#include "gui.hxx"
#include "jack.hxx"
#include "event.hxx"


char* processDspMem = 0;
char* processOscMem = 0;

jack_ringbuffer_t* rbToDsp = 0;
jack_ringbuffer_t* rbToGui = 0;

Jack* jack = 0;

int main()
{
  // allocate data to read from
  processDspMem = (char*)malloc( sizeof(EventBase) );
  processOscMem = (char*)malloc( sizeof(EventBase) );
  
  rbToDsp = jack_ringbuffer_create( 2000 * sizeof(EventBase));
  rbToGui = jack_ringbuffer_create( 2000 * sizeof(EventBase));
  
  
  jack = new Jack();
  
  jack->activate();
  
  Gui gui;
  gui.show();
  
}
