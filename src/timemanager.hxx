
#ifndef LUPPP_TIME_H
#define LUPPP_TIME_H

#include <iostream>
#include <cstdio>

#include "buffers.hxx"
#include "eventhandler.hxx"

#include "observer/time.hxx"

using namespace std;

class TimeManager
{
  public:
    TimeManager();
    
    int getFpb();
    void setBpm(float bpm);
    void setFpb(float f);
    
    void registerObserver(TimeObserver* o);
    
    void tap();
    
    void process(Buffers* buffers);
  
  private:
    int samplerate;
    
    float fpb;
    int oldBeat;
    
    // tap tempo measurements
    int frame;
    
    int tapTempoPos;
    int tapTempo[3];
    
    std::vector<TimeObserver*> observers;
};

#endif // LUPPP_TIME_H
