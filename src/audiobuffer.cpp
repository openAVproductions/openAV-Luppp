
#include "audiobuffer.hpp"

// static ID counter
int AudioBuffer::privateId = 0;

AudioBuffer::AudioBuffer()
{
  std::cout << "AudioBuffer::AudioBuffer()" << std::endl;
  // Set ID
  ID = privateId++;
}

int AudioBuffer::getID()
{
  return ID;
}

void AudioBuffer::setBeats(int beats)
{
  numBeats = beats;
  
  std::cout << "AudioBuffer::setBeats()" << std::endl;
}

int AudioBuffer::getBeats()
{
  return numBeats;
}

std::vector<float>* AudioBuffer::getPointer()
{
  //std::cout << "AudioBuffer::getPointer()  ID: " << ID << "  size = " << buffer.size() << std::endl;
  return &buffer;
}

AudioBuffer::~AudioBuffer()
{
  std::cout << "AudioBuffer::~AudioBuffer() Destroyed!" << std::endl;
}

