
#ifndef LUPPP_EVENT_HANDLER_DSP_H
#define LUPPP_EVENT_HANDLER_DSP_H


// Library
#include <cstring>
#include <iostream>
#include <jack/ringbuffer.h>

// Internal
#include "jack.hxx"
#include "event.hxx"
#include "eventhandler.hxx"

using namespace std;

extern Jack* jack;

void handleDspEvents()
{
  uint availableRead = jack_ringbuffer_read_space( rbToDsp );
  
  while ( availableRead >= sizeof(EventBase) )
  {
    jack_ringbuffer_peek( rbToDsp, (char*)processDspMem, sizeof(EventBase) );
    
    EventBase* e = (EventBase*)processDspMem;
    
    // recheck the size against the actual event size
    if ( availableRead >= e->size() )
    {
      switch ( e->type() )
      {
        case Event::MASTER_VOL: {
          if ( availableRead >= sizeof(EventMasterVol) ) {
            EventMasterVol ev(0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMasterVol) );
            //jack->masterVolume = ev.vol;
          } break; }
        case Event::LOAD_SAMPLE: {
          if ( availableRead >= sizeof(EventLoadSample) ) {
            EventLoadSample ev(0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLoadSample) );
            //jack->addAudioBuffer( ev.audioBufferPtr );
          } break; }
        case Event::PLAY_SAMPLE: {
          if ( availableRead >= sizeof(EventPlaySample) ) {
            EventPlaySample ev(0,0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventPlaySample) );
            //jack->setPlayBuffer( ev.track, ev.bufferID );
          } break; }
        case Event::RECORD: {
          if ( availableRead >= sizeof(EventRecord) ) {
            EventRecord ev(0,0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventRecord) );
            //jack->setRecord( ev.track, ev.record );
          } break; }
        default:
          {
            // just do nothing
            break;
          }
      }
    }
    else
    {
      // next call will get the half-written event
      return;
    }
    
    // update available read, and loop over events
    availableRead = jack_ringbuffer_read_space( rbToDsp );
  }
}

void writeToDspRingbuffer(EventBase* e)
{
  if ( jack_ringbuffer_write_space(rbToDsp) >= e->size() )
  {
    jack_ringbuffer_write( rbToDsp, (const char*)e, e->size() );
  }
  else
  {
    cout << "->DSP ringbuffer full!" << endl;
  }
}

#endif // LUPPP_EVENT_HANDLER_DSP_H

