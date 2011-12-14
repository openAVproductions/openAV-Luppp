
#ifndef LUPPP_AUDIOBUFFER
#define LUPPP_AUDIOBUFFER

#include <iostream>
#include <vector>
#include <glibmm.h>

class AudioBuffer
{
  public:
    
    AudioBuffer();
    ~AudioBuffer();
    
    int getID();
    
    int  getBeats();
    void setBeats(int);
    
    std::vector<float>* getPointer();
  
  protected:
    
    static int privateId;
    int ID;
    
    int numBeats;
    
    std::vector<float> buffer;
};

#endif

