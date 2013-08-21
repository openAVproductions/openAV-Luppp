
#include "looperclip.hxx"

#include <stdio.h>
#include "config.hxx"
#include "event.hxx"
#include "eventhandler.hxx"
#include "audiobuffer.hxx"

LooperClip::LooperClip()
{
  _state = GridLogic::STATE_EMPTY;
  
  _loaded = false;
  _playing = false;
  _recording = false;
  
  _buffer = 0; //new AudioBuffer(44100);
  _newBufferInTransit = false;
  
  _playhead   = 0;
  _recordhead = 0;
}

/// loads a sample: eg from disk, unloading current sample if necessary
void LooperClip::load( AudioBuffer* ab )
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

void LooperClip::setRequestedBuffer( AudioBuffer* ab )
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


void LooperClip::record(int count, float* L, float* R)
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

unsigned long LooperClip::recordSpaceAvailable()
{
  if ( _buffer )
    return _buffer->getData().size() - _recordhead;
  
  return 0;
}

size_t LooperClip::audioBufferSize()
{
  if ( _buffer )
  {
    return _buffer->getData().size();
  }
  return 0;
}

void LooperClip::setBeats(int beats)
{
  if ( _buffer )
  {
    _buffer->setBeats( beats );
  }
}

int LooperClip::getBeats()
{
  if ( _buffer )
    return _buffer->getBeats();
  
  return 0;
}

long LooperClip::getBufferLenght()
{
  return _recordhead;
}

bool LooperClip::loaded()
{
  return _loaded;
}

void LooperClip::play()
{
  _playing = true;
  _playhead = 0;
}
void LooperClip::stop()
{
  _playing = false;
  _playhead = 0;
}

bool LooperClip::playing()
{
  return _playing;
}

bool LooperClip::recording()
{
  return _recording;
}


void LooperClip::record()
{
  /*
  if ( !r && duration )
  {
    setBeats( duration );
  }
  */
  _recording = true;
  _playing = false;
}


void LooperClip::newBufferInTransit(bool n){_newBufferInTransit = n;}
bool LooperClip::newBufferInTransit(){return _newBufferInTransit;}

float LooperClip::getSample(float playSpeed)
{
  if ( _buffer && _buffer->getData().size() > 0 )
  {
    if ( _playhead >= _recordhead ||
         _playhead >= _buffer->getData().size() ||
         _playhead < 0  )
    {
      _playhead = 0;
      
      EventGuiPrint e( "LooperClip resetting _playhead" );
      writeToGuiRingbuffer( &e );
    }
    
    std::vector<float>& v = _buffer->getData();
    float tmp = v.at(_playhead);
    _playhead += playSpeed;
    
    return tmp;
  }
  
  return 0.f;
}

float LooperClip::getProgress()
{
  if ( _buffer && _playing )
  {
    float p = float(_playhead) / _recordhead;
    //printf("LooperClip progress %f\n", p );
    return p;
  }
  return 0.f;
}
