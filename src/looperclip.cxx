
#include "looperclip.hxx"

#include <stdio.h>
#include "config.hxx"
#include "jack.hxx"
#include "event.hxx"
#include "eventhandler.hxx"
#include "audiobuffer.hxx"

extern Jack* jack;

LooperClip::LooperClip(int t, int s) :
  SaveAble(),
  track(t),
  scene(s)
{
  _loaded     = false;
  _playing    = false;
  _recording  = false;
  
  _queuePlay  = false;
  _queueStop  = false;
  _queueRecord= false;
  
  _buffer = new AudioBuffer(4410);
  _newBufferInTransit = false;
  
  _playhead   = 0;
  _recordhead = 0;
}


void LooperClip::save()
{
  // ensure there is something in the buffer to be saved
  if ( _loaded )
  {
    char buffer [50];
    sprintf (buffer, "LC::save() track %i, scene %i", track,scene);
    EventGuiPrint e( buffer );
    writeToGuiRingbuffer( &e );
    
    EventRequestSaveBuffer e2( track, scene, _buffer->getAudioFrames() );
    writeToGuiRingbuffer( &e2 );
  }
  else
  {
    SaveAble::done();
  }
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
  
  char buffer [50];
  sprintf (buffer, "LC::load() t %i, s %i, aF %i",track, scene, int(_buffer->getAudioFrames()) );
  EventGuiPrint e( buffer );
  writeToGuiRingbuffer( &e );
  
}

void LooperClip::setRequestedBuffer( AudioBuffer* ab )
{
  if ( _buffer )
  {
    size_t size = _buffer->getData().size();
    memcpy( &ab->getData().at(0), &_buffer->getData().at(0), sizeof(float)*size);
    
    ab->setID   ( _buffer->getID()    );
    ab->setBeats( _buffer->getBeats() );
    
    EventDeallocateBuffer e( _buffer );
    writeToGuiRingbuffer( &e );
  }
  
  _buffer = ab;
  
  _newBufferInTransit = false;
}



void LooperClip::recieveSaveBuffer( AudioBuffer* saveBuffer )
{
  // copy current contents into save buffer,
  size_t framesBySize = _buffer->getAudioFrames();
  memcpy( &saveBuffer->getData().at(0), &_buffer->getData().at(0), sizeof(float)*framesBySize);
  
  saveBuffer->setID   ( _buffer->getID()    );
  saveBuffer->setBeats( _buffer->getBeats() );
  saveBuffer->setAudioFrames( _buffer->getAudioFrames() );
  
  EventSaveBuffer e ( track, scene, saveBuffer );
  writeToGuiRingbuffer( &e );
  
  SaveAble::done();
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
  
  _loaded = true;
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

void LooperClip::bar()
{
  bool change = false;
  GridLogic::State s = GridLogic::STATE_EMPTY;
  
  if ( _queuePlay && _loaded )
  {
    _playing = true;
    s = GridLogic::STATE_PLAYING;
    _recording = false;
    _queuePlay = false;
    change = true;
    
    _playhead = 0;
  }
  else if ( _queueStop && _loaded )
  {
    _playing   = false;
    s = GridLogic::STATE_STOPPED;
    _recording = false;
    _queueStop = false;
    change = true;
    // set "progress" to zero, as we're stopped!
    jack->getControllerUpdater()->setTrackSceneProgress(track, scene, 0 );
  }
  else if ( _queueRecord )
  {
    _recording   = true;
    s = GridLogic::STATE_RECORDING;
    _playing     = false;
    _queueRecord = false;
    change = true;
    
    if ( _buffer )
    {
      _buffer->setBeats( 0 );
    }
    
    _recordhead = 0;
  }
  else if ( _queuePlay )
  {
    // clip was queued, but there's nothing loaded
    _queuePlay = false;
    change = true;
  }
  
  if ( _recording )
  {
    // FIXME: assumes 4 beats in a bar
    _buffer->setBeats( _buffer->getBeats() + 4 );
    _buffer->setAudioFrames( jack->getTimeManager()->getFpb() * _buffer->getBeats() );
  }
  
  if ( change )
  {
    jack->getControllerUpdater()->setSceneState(track, scene, s );
  }
}


void LooperClip::queuePlay()
{
  _queuePlay   = true;
  _queueStop   = false;
}

void LooperClip::queueStop()
{
  _queueStop   = true;
  _queuePlay   = false;
}

void LooperClip::queueRecord()
{
  _queueRecord = true;
  _queuePlay   = false;
  _queueStop   = false;
}

GridLogic::State LooperClip::getState()
{
  GridLogic::State s = GridLogic::STATE_EMPTY;
  
  if ( _loaded       )
    s = GridLogic::STATE_STOPPED;
  if ( _playing      )
    s = GridLogic::STATE_PLAYING;
  if ( _recording    )
    s = GridLogic::STATE_RECORDING;
  if ( _queuePlay    )
    s = GridLogic::STATE_PLAY_QUEUED;
  if ( _queueStop    )
    s = GridLogic::STATE_STOP_QUEUED;
  if ( _queueRecord  )
    s = GridLogic::STATE_RECORD_QUEUED;
  
  return s;
}

bool LooperClip::playing()
{
  return _playing;
}

bool LooperClip::recording()
{
  return _recording;
}

void LooperClip::newBufferInTransit(bool n)
{
  _newBufferInTransit = n;
}

bool LooperClip::newBufferInTransit()
{
  return _newBufferInTransit;
}

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
