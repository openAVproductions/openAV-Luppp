

// Library
#include <iostream>
#include <unistd.h>

#include <jack/ringbuffer.h>

// Internal
#include "gui.hxx"
#include "jack.hxx"


char* processDspMem = 0;
char* processOscMem = 0;

jack_ringbuffer_t* rbToDsp = 0;
jack_ringbuffer_t* rbToGui = 0;

Jack* jack = 0;

int main()
{
  
  jack = new Jack();
  
  jack->activate();
  
  Gui gui;
  gui.show();
  
}
