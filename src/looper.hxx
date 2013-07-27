
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
#include <iostream>

#include "buffers.hxx"
#include "audioprocessor.hxx"
#include "observer/observer.hxx"

class AudioBuffer;

using namespace std;

/** LooperClip
 * Represents each clip that a looper can playback. The core of the audio
 * samples is stored in AudioBuffer objects which are dynamically resized. The
 * base size of a AudioBuffer is 1 second's worth, after which larger buffers
 * will be requested.
 * The transition between AudioBuffer instances is seamless: when the clip is
 * running out, the new one is requested. Upon its arrival the current data is
 * copied, and the old buffer is returned for deallocation.
 * 
 * This system allows for arbitrary length recordings, without huge
 * pre-allocated buffers while it is still quite simple.
 * 
 * Each clip has its properties like length and bars/beats, so the Looper knows
 * to dynamically stretch / process the audio appropriately. Controllers and the
 * UI are updated from this data.
**/
class LooperClip
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
    
    LooperClip()
    {
      _loaded = false;
      _buffer = 0;
      _state  = STATE_STOPPED;
    }
    
    void setRequestedBuffer( AudioBuffer* ab )
    {
      // here we copy the data from the existing buffer into the new one,
      // and send the old one away to be deallocated.
    }
    
    bool  loaded(){return _loaded;}
    State state(){return _state;}
    
    // Set
    void clipLength(int l){_clipLenght = l;}
  
  private:
    bool _loaded;
    State _state;
    AudioBuffer* _buffer;
    
    // Clip Properties
    int _clipLenght;
};

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
    
    void setFpb(int f) { fpb = f; }
    
    void setScene( int sc );
    //void setState( State s);
    
    
    void updateControllers();
    void process(int nframes, Buffers* buffers);
  
  private:
    const int track;
    //State state;
    int scene;
    
    int fpb;
    float gain;
    int numBeats;
    int playedBeats;
    bool stopRecordOnBar;
    
    int endPoint, lastWrittenSampleIndex;
    float playPoint;
    float* sample;
    float* tmpRecordBuffer;
    
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
