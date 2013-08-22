
#ifndef LUPPP_AUDIOBUFFER_H
#define LUPPP_AUDIOBUFFER_H

#include <vector>
#include <iostream>

/// AudioBuffer stores float samples in a big vector.
class AudioBuffer
{
  public:
    AudioBuffer()
    {
      ID = privateID++;
      numBeats = 0;
    }
    AudioBuffer(unsigned long size)
    {
      //ID = id;
      numBeats = 0;
      buffer.resize(size);
    }
    
    
    int getID()
    {
      return ID;
    }
    
    int getBeats()
    {
      return numBeats;
    }
    
    void setBeats(int b)
    {
      printf("AudioBuffer now has %i beats\n", b );
      numBeats = b;
    }
    
    std::vector<float>& getData()
    {
      return buffer;
    }
    
    void nonRtSetSample(std::vector<float>& sample)
    {
      buffer.swap(sample);
    }
  
  protected:
    static int privateID;
    int ID;
    
    int numBeats;
    
    std::vector<float> buffer;
};

#endif

