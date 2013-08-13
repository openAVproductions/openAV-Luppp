
#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

#include <stdio.h>
#include "config.hxx"
#include "eventhandler.hxx"
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
      
      _buffer = 0; //new AudioBuffer(44100);
      _newBufferInTransit = false;
      
      _playhead   = 0;
      _recordhead = 0;
    }
    
    /// loads a sample: eg from disk, unloading current sample if necessary
    void load( AudioBuffer* ab )
    {
      _loaded = true;
      
      if ( _buffer )
      {
        EventDeallocateBuffer e( _buffer );
        writeToGuiRingbuffer( &e );
      }
      
      _buffer = ab;
      
      _playhead = 0;
      
      // set the endpoint to the buffer's size
      _recordhead = _buffer->getData().size();
    }
    
    /// used to update the size of the buffer for this looperclip. The current
    /// data is copied into the new buffer, then the smaller buffer is sent
    /// for de-allocation
    void setRequestedBuffer( AudioBuffer* ab )
    {
      if ( _buffer )
      {
        size_t size = _buffer->getData().size();
        
        for(size_t i = 0; i < size; i++)
        {
          ab->getData().at(i) = _buffer->getData().at( i );
        }
        
        EventDeallocateBuffer e( _buffer );
        writeToGuiRingbuffer( &e );
      }
      
      _buffer = ab;
      
      _newBufferInTransit = false;
    }
    
    void record(int count, float* L, float* R)
    {
      // write "count" samples into current buffer.
      if ( _buffer )
      {
        for(int i = 0; i < count; i++)
        {
          _buffer->getData().at( _recordhead ) = *L++;
          _recordhead++;
        }
      }
      
    }
    
    unsigned long recordSpaceAvailable()
    {
      if ( _buffer )
        return _buffer->getData().size() - _recordhead;
      
      return 0;
    }
    
    size_t audioBufferSize()
    {
      if ( _buffer )
      {
        return _buffer->getData().size();
      }
      return 0;
    }
    
    void setBeats(int beats)
    {
      if ( _buffer )
      {
        _buffer->setBeats( beats );
      }
    }
    
    int getBeats()
    {
      if ( _buffer )
        return _buffer->getBeats();
      
      return 0;
    }
    
    long getBufferLenght()
    {
      return _recordhead;
    }
    
    bool loaded(){return _loaded;}
    void playing(bool p){_playing = p; _playhead = 0; }
    bool playing(){return _playing;}
    bool recording(){return _recording;}
    void recording(bool r) {_recording = r;}
    
    void newBufferInTransit(bool n){_newBufferInTransit = n;}
    bool newBufferInTransit(){return _newBufferInTransit;}
    
    float getSample(float playSpeed)
    {
      if ( _buffer )
      {
        if ( _playhead >= _recordhead ||
             _playhead >= _buffer->getData().size() ||
             _playhead < 0  )
        {
          _playhead = 0;
          printf("looper resetting playhead\n");
        }
        
        std::vector<float>& v = _buffer->getData();
        float tmp = v.at(_playhead);
        _playhead += playSpeed;
        
        return tmp;
      }
      
      return 0.f;
    }
    
    float getProgress()
    {
      if ( _buffer && _playing )
      {
        float p = float(_playhead) / _recordhead;
        printf("LooperClip progress %f\n", p );
        return p;
      }
      return 0.f;
    }
  
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

