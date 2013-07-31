
#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

#include <stdio.h>
#include "audiobuffer.hxx"

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
    LooperClip()
    {
      _loaded = false;
      _playing = false;
      _recording = false;
      
      _buffer = 0;
      
      _index = 0;
    }
    
    // loads a sample: eg from disk
    void load( AudioBuffer* ab )
    {
      _loaded = true;
      
      if ( _buffer )
      {
        // unload old buffer!
        printf("LooperClip: FIXME:TODO de-allocate old sample here!\n");
      }
      
      _buffer = ab;
      
      _index = 0;
      
      _playing = true;
    }
    
    void setRequestedBuffer( AudioBuffer* ab )
    {
      // here we copy the data from the existing buffer into the new one,
      // and send the old one away to be deallocated.
    }
    
    void record(int count, float* L, float* R)
    {
      // write "count" samples into current buffer. If the last
    }
    
    
    int nframesAvailable()
    {
      // return amount of space left to record
      return 0;
    }
    
    
    bool loaded(){return _loaded;}
    bool playing(){return _playing;}
    bool recording(){return _recording;}
    
    float getSample()
    {
      if ( _buffer )
      {
        if ( _index >= _buffer->getData().size() || _index < 0  )
        {
          _index = 0;
        }
        float tmp = _buffer->getData().at(_index);
        _index++;
        
        return tmp;
      }
      
      return 0.f;
    }
    
    float getProgress()
    {
      if ( _buffer )
      {
        return float(_index) / _buffer->getData().size();
      }
      return 0.f;
    }
    
    // Set
    void clipLength(int l){_clipLenght = l;}
  
  private:
    bool _loaded;
    bool _recording;
    bool _playing;
    
    unsigned int _index;
    AudioBuffer* _buffer;
    
    // Clip Properties
    int _clipLenght;
};

#endif // LUPPP_LOOPER_CLIP_H

