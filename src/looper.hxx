
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
#include <iostream>

#include "buffers.hxx"
#include "audioprocessor.hxx"
#include "observer/observer.hxx"

#define SAMPLE_SIZE 44100*20

class AudioBuffer;

using namespace std;

class Looper : public Observer, public AudioProcessor
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
    
    void setSample(int c, AudioBuffer* ab);
    
    void midi(unsigned char* data);
    
    void bar();
    void beat();
    
    void setFpb(int f) { fpb = f; }
    
    void setScene( int sc );
    void setState( State s);
    void setLoopLength(float l);
    
    void updateControllers();
    void process(int nframes, Buffers* buffers);
  
  private:
    const int track;
    State state;
    int scene;
    
    int fpb;
    float gain;
    int numBeats;
    int playedBeats;
    bool stopRecordOnBar;
    
    int endPoint, lastWrittenSampleIndex;
    float playPoint;
    float* sample;
    float samples[10][SAMPLE_SIZE];
    
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
    
    int uiUpdateConstant;
    int uiUpdateCounter;
};

#endif // LUPPP_LOOPER_H
