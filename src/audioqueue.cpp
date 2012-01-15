
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  
  //cout << "AudioQueue::pull() availableRead = " << availableRead << "  writeLocation " << writeBuffer << endl;
  
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
  return availableReadBytes / sizeof(float);
}


AudioQueue::~AudioQueue()
{
  std::cout << "~AudioQueue()" << std::endl;
  jack_ringbuffer_free(buffer);
}

