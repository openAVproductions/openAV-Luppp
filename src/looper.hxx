
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
#include <iostream>

#include "buffers.hxx"

#include "audioprocessor.hxx"

#include "looperclip.hxx"
#include "observer/observer.hxx"

using namespace std;

/** Looper
 * The class which reads from LooperClips, and reads/ writes the data using the
 * track buffer. Scene recording / playback is the essential functionality here.
**/
class Looper : public Observer, public AudioProcessor
{
  public:
    Looper(int t);
    
    void setSample(int c, AudioBuffer* ab);
    
    void midi(unsigned char* data);
    
    void bar();
    void beat();
    
    void setFpb(int f) { /*fpb = f;*/ }
    
    void setScene( int sc );
    
    
    LooperClip* getClip(int scene);
    
    void updateControllers();
    void process(int nframes, Buffers* buffers);
  
  private:
    const int track;
    
    float* tmpRecordBuffer;
    LooperClip clips[10];
    
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
