
#ifndef LUPPP_FILEAUDIOSOURCE
#define LUPPP_FILEAUDIOSOURCE

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <sndfile.hh>

#include "audiosource.hpp"

#include "fileaudiosourcestate.hpp"

#include "top.hpp"

using std::string;

class FileAudioSource : public AudioSource
{
  public:
    FileAudioSource(Top*,string name);
    
    void setParameter(int p,float s);
    void process (int, float* );
  
  private:
    Top* top;
    int ID;
};


#endif
