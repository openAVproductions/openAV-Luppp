
#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

#include <stdio.h>
#include "config.hxx"
#include "gridlogic.hxx"

class AudioBuffer;

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
    LooperClip(int track, int scene);
    
    /// loads a sample: eg from disk, unloading current sample if necessary
    void load( AudioBuffer* ab );
    
    /// audio functionality
    float getSample(float playSpeed);
    void record(int count, float* L, float* R);
    
    /// TimeObserver overrides
    void bar();
    
    /// analyses current _playing _recording vars, returns the current State
    GridLogic::State getState();
    
    /// get buffer details
    int   getBeats();
    float getProgress();
    long  getBufferLenght();
    size_t audioBufferSize();
    
    /// set clip state
    void  queuePlay();
    void  queueStop();
    void  queueRecord();
    
    /// set buffer state
    void setBeats(int beats);
    
    /// Luppp internal buffer resizing
    void newBufferInTransit(bool n);
    bool newBufferInTransit();
    size_t recordSpaceAvailable();
    
    /** used to update the size of the buffer for this looperclip. The current
     *  data is copied into the new buffer, then the smaller buffer is sent
     *  for de-allocation.
    **/
    void setRequestedBuffer( AudioBuffer* ab );
  
  private:
    int track, scene;
    
    /** Luppp needs more than the current state of the clip to accuratly handle
     *  it. The current state of the grid is kept up-to-date by GridLogic
     *  abstracting detail away, sending GridLogic::State to Controllers.
    **/
    bool _loaded;
    bool _playing;
    bool _recording;
    
    bool _queuePlay;
    bool _queueStop;
    bool _queueRecord;
    
    bool _newBufferInTransit;
    
    float _playhead;
    float _recordhead;
    AudioBuffer* _buffer;
};

#endif // LUPPP_LOOPER_CLIP_H

