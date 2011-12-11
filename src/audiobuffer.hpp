
#ifndef LUPPP_AUDIOBUFFER
#define LUPPP_AUDIOBUFFER

//#include "globals.hpp"

#include <iostream>
#include <vector>
#include <glibmm.h>

// #include <gtkmm.h>

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
    std::string name;
    
    /*
    int index;
    int channels;
    int samplerate;
    */
    
    std::vector<float> buffer;
};

#endif

