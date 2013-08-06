
#ifndef LUPPP_AUDIOBUFFER_H
#define LUPPP_AUDIOBUFFER_H

// System
#include <vector>

// AudioBuffer stores float samples in a big vector. The vector can be
// accessed only by const reference, so its state is immutable
class AudioBuffer
{
  public:
    AudioBuffer()
    {
      //ID = privateID++;
    }
    AudioBuffer(unsigned long size)
    {
      //ID = id;
      buffer.resize(size);
    }
    ~AudioBuffer();
    /*
    int getID()
    {
      return ID;
    }
    */
    int getBeats()
    {
      return numBeats;
    }
    
    void setBeats(int b)
    {
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
    //static int privateID;
    //int ID;
    
    int numBeats;
    
    std::vector<float> buffer;
};

#endif

