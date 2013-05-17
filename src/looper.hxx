
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
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
    
    Looper(int t);
    
    void bar();
    void beat();
    
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
    bool stopRecordOnBar;
    
    int endPoint, playPoint, lastWrittenSampleIndex;
    float sample[44100*60];
    
    // Pitch Shifting
    void pitchShift(int count, float* input, float* output);
    vector<float> tmpBuffer;
    int    IOTA;
    float  fVec0[65536];
    float  semitoneShift;
    float  windowSize;
    float  fRec0[2];
    float  crossfadeSize;
    float  fSamplingFreq;
};

#endif // LUPPP_LOOPER_H
