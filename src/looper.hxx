
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <iostream>

#include "buffers.hxx"
#include "observer/observer.hxx"

using namespace std;

class Looper : public Observer // for notifications
{
  public:
    enum State {
      STATE_PLAYING = 0,
      STATE_PLAY_QUEUED,
      STATE_RECORDING,
      STATE_RECORD_QUEUED,
      STATE_STOPPED,
      STATE_STOP_QUEUED,
    };
    
    Looper(int t) :
      track(t),
      state(STATE_STOPPED),
      numBeats   (4),
      playedBeats(0),
      endPoint   (0),
      playPoint  (0),
      lastWrittenSampleIndex(0)
    {}
    
    void bar();
    
    void beat() { playedBeats++; }
    void setFpb(int f) { fpb = f; }
    
    void setState(State s);
    void setLoopLength(float l);
    
    void process(int nframes, Buffers* buffers);
  
  private:
    int track;
    State state;
    
    int fpb;
    int numBeats;
    int playedBeats;
    
    int endPoint, playPoint, lastWrittenSampleIndex;
    float sample[44100*60];
  
};

#endif // LUPPP_LOOPER_H
