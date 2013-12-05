
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
    
    /// add a component to be updated for time events
    void registerObserver(TimeObserver* o);
    
    /// call this when a tempo-tap occurs
    void tap();
    
    /// called to process buffers->nframes samples. If a beat is present, this
    /// is handled gracefully, first calling process up to the beat, then doing
    /// a beat() event on all TimeObservers, and processing the remaining samples
    void process(Buffers* buffers);
    
    /// returns the number of samples till beat if a beat exists in this process
    /// Otherwise returns nframes
    int getNframesToBeat();
  
  private:
    int samplerate;
    
    /// number of frames per beat
    float fpb;
    
    /// holds the number of frames processed
    long long totalFrameCounter;
    
    /// counts down frames until the next beat
    long beatFrameCountdown;
    
    /// counts bars / beats processed
    int barCounter;
    int beatCounter;
    
    /// tap tempo measurements
    int frame;
    
    int tapTempoPos;
    int tapTempo[3];
    
    std::vector<TimeObserver*> observers;
};

#endif // LUPPP_TIME_H
