
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
    void setBpmZeroOne(float bpm);
    void setFpb(float f);
    
    void registerObserver(TimeObserver* o);
    
    void tap();
    
    void process(Buffers* buffers);
    
    /// returns the number of samples till beat if a beat exists in this process
    /// Otherwise returns nframes
    int getNframesToBeat();
    bool beatInThisProcess();
  
  private:
    int samplerate;
    
    /// holds the number of frames before a beat
    //int nframesToBeat;
    bool beatInProcess;
    
    /// counts down frames until the next beat
    int beatFrameCountdown;
    
    float fpb;
    int beatCounter;
    
    /// tap tempo measurements
    int frame;
    
    int tapTempoPos;
    int tapTempo[3];
    
    std::vector<TimeObserver*> observers;
};

#endif // LUPPP_TIME_H
