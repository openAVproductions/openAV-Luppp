
#ifndef LUPPP_METRONOME_H
#define LUPPP_METRONOME_H

#include <cmath>
#include <iostream>

#include "buffers.hxx"

#include "observer/time.hxx"

using namespace std;

class Metronome : public TimeObserver
{
  public:
    Metronome();
    ~Metronome(){};
    
    void setActive(bool a);
    
    void bar();
    void beat();
    void setFpb(int f);
    
    void process(int nframes, Buffers* buffers);
  
  private:
    int fpb;
    bool playBar;
    bool active;
    
    int playPoint, endPoint;
    float barSample[44100];
    float beatSample[44100];
  
};

#endif // LUPPP_METRONOME_H
