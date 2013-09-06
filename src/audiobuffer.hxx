
#ifndef LUPPP_AUDIOBUFFER_H
#define LUPPP_AUDIOBUFFER_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

/// AudioBuffer stores float samples in a big vector.
class AudioBuffer
{
  public:
    AudioBuffer()
    {
      ID = privateID++;
      init();
    }
    AudioBuffer(unsigned long size)
    {
      // FIXME recorded buffers don't get an ID, using garbage IDs 
      /// no ID assigned: it *takes* the one from the previous buffer!
      init();
      buffer.resize(size);
    }
    
    void init()
    {
      numBeats = 0;
      audioFrames = 0;
    }
    
    /// this function is used for "resizing" an exisiting buffer, and should
    /// not be called for any other reason.
    void setID(int id)
    {
      ID = id;
    }
    
    int getID()
    {
      return ID;
    }
    
    void setName(const char* n)
    {
      if ( strlen(n) > 19 )
      {
#ifdef DEBUG_BUFFER
        cout << "AudioBuffer setName too long!" << endl;
#endif
        return;
      }
      
      memcpy( name, n, sizeof(char)* 20 ); 
    }
    
    int getBeats()
    {
      return numBeats;
    }
    
    void setBeats(int b)
    {
#ifdef DEBUG_BUFFER
      cout << "AudioBuffer now has " << b << " beats\n" << endl;
#endif
      numBeats = b;
    }
    
    void setAudioFrames(long af)
    {
      audioFrames = af;
#ifdef DEBUG_BUFFER
      cout << "AudioBuffer " << ID << " has "  << audioFrames << " audioFrames\n" << endl;
#endif
    }
    
    long getAudioFrames()
    {
      return audioFrames;
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
    
    /// holds the number of samples that are usable audio, as opposed to
    /// buffer.size(), which also has non-used space at the end.
    long audioFrames;
    
    char name[20];
    
    std::vector<float> buffer;
};

#endif

