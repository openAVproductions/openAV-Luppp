
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
#include <iostream>

#include "buffers.hxx"

#include "audioprocessor.hxx"

#include "looperclip.hxx"
#include "observer/time.hxx"

using namespace std;

/** Looper
 * The class which reads from LooperClips, and reads/ writes the data using the
 * track buffer. Scene recording / playback is the essential functionality here.
**/
class Looper : public AudioProcessor, public TimeObserver
{
  public:
    Looper(int t);
    
    /// *sets* the sample
    void setSample(int c, AudioBuffer* ab);
    
    /// *sets* the new audiobuffer, but the content gets copied to the new buffer.
    /// Used for infinite lenght recording
    void setRequestedBuffer(int s, AudioBuffer* ab);
    
    void midi(unsigned char* data);
    
    void bar();
    void beat();
    
    void setFpb(int f) { /*fpb = f;*/ }
    
    void queuePlayScene( int sc );
    
    
    LooperClip* getClip(int scene);
    
    void updateControllers();
    void process(int nframes, Buffers* buffers);
  
  private:
    const int track;
    
    /// variables used to determing the current actions of the looper
    int playingScene;
    int queuedScene;
    
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
