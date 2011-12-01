
#include "rtqueue.hpp"

using namespace std;

RtQueue::RtQueue()
{
  //std::cout << "RtQueue()" << std::flush;
  
  // initialize the ringbuffer to 100 elements
  buffer = jack_ringbuffer_create( 2000 * sizeof(EngineEvent));
  
  if ( buffer == 0 )
  {
    cout << "Error creating RtQueue!" << endl;
  }
  
  // lock the buffer into memory
  int res = jack_ringbuffer_mlock(buffer);
  if ( res )
  {
    std::cout << "RtQueue() Error locking memory!" << std::endl;
  }
  
  tempEvent = new EngineEvent;
}

void RtQueue::push(EngineEvent *ev)
{
  int availableWrite = jack_ringbuffer_write_space(buffer);
  if (availableWrite >= sizeof(EngineEvent))
  {
    int written = jack_ringbuffer_write( buffer, (const char*) ev , sizeof(EngineEvent) );
    
    if (written != sizeof(EngineEvent) )
    {
      std::cout << "RtQueue::push() ERROR! didn't write full event!" << std::endl;
    }
  }
  else
  {
    std::cout << "RtQueue::push() ERROR! RingBuffer FULL! Skipping event..." <<std::endl;
  }
}

EngineEvent* RtQueue::pull()
{
  // check if there's anything to read
  int availableRead = jack_ringbuffer_read_space(buffer);
  
  if ( availableRead >= (long)sizeof(EngineEvent) )
  {
    // read from the buffer
    int result = jack_ringbuffer_read(buffer, (char*)tempEvent, sizeof(EngineEvent));
    
    if ( result != sizeof(EngineEvent) )
      std::cout << "RtQueue::pull() WARNING! didn't read full event!" << std::endl;
    
    // return the pointer to the event
    return tempEvent;
  }
  
  // nothing available
  return 0;
}

RtQueue::~RtQueue()
{
  std::cout << "~RtQueue()" << std::endl;
  jack_ringbuffer_free(buffer);
}

