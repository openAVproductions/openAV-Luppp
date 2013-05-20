
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
        case Event::LOOPER_LOAD: {
          if ( availableRead >= sizeof(EventLooperLoad) ) {
            EventLooperLoad ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperLoad) );
            Looper* l = jack->getLooper( ev.track );
            //assert(l);
            l->setSample( ev.clip, (AudioBuffer*)ev.audioBuffer );
          } break; }
        case Event::PLAY_SAMPLE: {
          if ( availableRead >= sizeof(EventPlaySample) ) {
            EventPlaySample ev(0,0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventPlaySample) );
            //jack->setPlayBuffer( ev.track, ev.bufferID );
          } break; }
        case Event::METRONOME_ACTIVE: {
          if ( availableRead >= sizeof(EventMetronomeActive) ) {
            EventMetronomeActive ev(false);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMetronomeActive) );
            jack->getMetronome()->setActive(ev.active);
          } break; }
        case Event::LOOPER_STATE: {
          if ( availableRead >= sizeof(EventLooperState) ) {
            EventLooperState ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperState) );
            jack->setLooperState( ev.track, ev.state );
          } break; }
        case Event::LOOPER_LOOP_LENGTH: {
          if ( availableRead >= sizeof(EventLooperLoopLength) ) {
            EventLooperLoopLength ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperLoopLength) );
            jack->setLooperLoopLength( ev.track, ev.scale );
          } break; }
        case Event::TIME_BPM: {
          if ( availableRead >= sizeof(EventTimeBPM) ) {
            EventTimeBPM ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventTimeBPM) );
            jack->getTimeManager()->setBpm(ev.bpm);
          } break; }
        case Event::TIME_TEMPO_TAP: {
          if ( availableRead >= sizeof(EventTimeTempoTap) ) {
            EventTimeTempoTap ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventTimeTempoTap) );
            jack->getTimeManager()->tap();
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

