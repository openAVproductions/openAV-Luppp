
#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

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

#endif // LUPPP_LOOPER_CLIP_H

