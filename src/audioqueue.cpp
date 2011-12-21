
#include "audioqueue.hpp"

using namespace std;

AudioQueue::AudioQueue()
{
  //std::cout << "AudioQueue()" << std::flush;
  
  // initialize the ringbuffer to 100 elements
  buffer = jack_ringbuffer_create( 44100 * 100 * sizeof(float));
  
  if ( buffer == 0 )
  {
    cout << "Error creating AudioQueue!" << endl;
  }
  
  // lock the buffer into memory
  int res = jack_ringbuffer_mlock(buffer);
  if ( res )
  {
    std::cout << "AudioQueue() Error locking memory!" << std::endl;
  }
  
  tempEvent = new float[100*44100];
}

void AudioQueue::push(int nframes, float* audioPointer)
{
  unsigned int availableWrite = jack_ringbuffer_write_space(buffer);
  
  if (availableWrite >= sizeof(float) * nframes )
  {
    unsigned int written = jack_ringbuffer_write( buffer, (const char*)audioPointer , sizeof(float) * nframes );
    
    if (written != sizeof(float) * nframes )
    {
      std::cout << "AudioQueue::push() ERROR! didn't write full event!" << std::endl;
    }
  }
  else
  {
    std::cout << "AudioQueue::push() ERROR! RingBuffer FULL! Skipping event..." <<std::endl;
  }
}

int AudioQueue::writeSamplesTo(float *writeBuffer)
{
  // check if there's anything to read
  int availableRead = jack_ringbuffer_read_space(buffer);
  
  cout << "AudioQueue::pull() availableRead = " << availableRead << "  writeLocation " << writeBuffer << endl;
  
  int returnValue = 0;
  
  if ( availableRead >= (long)sizeof(float) )
  {
    // read from the buffer
    int result = jack_ringbuffer_read(buffer, (char*)writeBuffer, sizeof(float) * availableRead);
    
    if ( result != availableRead )
    {
      std::cout << "AudioQueue::pull() WARNING! didn't read all contents! Result = " << result << std::endl;
      returnValue = -1;
    }
  }
  
  // reset the buffer, so that we start writing @ the start again next time
  jack_ringbuffer_reset(buffer);
  
  // nothing available
  return returnValue;
}

// this function checks if the GUI thread should push events to the Queue
// so that the JACK thread will always have blank EE*'s available
// it should *not* be used for any other purpose!
int AudioQueue::readSpaceAvailable()
{
  int availableReadBytes = (int)jack_ringbuffer_read_space(buffer);
  return availableReadBytes;
}


AudioQueue::~AudioQueue()
{
  std::cout << "~AudioQueue()" << std::endl;
  jack_ringbuffer_free(buffer);
}

