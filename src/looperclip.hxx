
#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

#include <stdio.h>
#include "config.hxx"

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
    LooperClip();
    
    /// loads a sample: eg from disk, unloading current sample if necessary
    void load( AudioBuffer* ab );
    
    /// used to update the size of the buffer for this looperclip. The current
    /// data is copied into the new buffer, then the smaller buffer is sent
    /// for de-allocation
    void setRequestedBuffer( AudioBuffer* ab );
    
    void record(int count, float* L, float* R);
    
    unsigned long recordSpaceAvailable();
    
    size_t audioBufferSize();
    
    void setBeats(int beats);
    
    int getBeats();
    
    long getBufferLenght();
    
    bool loaded();
    void playing(bool p);
    bool playing();
    bool recording();
    void recording(bool r);
    
    void newBufferInTransit(bool n);
    bool newBufferInTransit();
    
    float getSample(float playSpeed);
    
    float getProgress();
  
  private:
    bool _loaded;
    bool _recording;
    bool _playing;
    
    bool _newBufferInTransit;
    
    float _playhead;
    float _recordhead;
    AudioBuffer* _buffer;
};

#endif // LUPPP_LOOPER_CLIP_H

