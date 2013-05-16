
#ifndef LUPPP_EVENT_HANDLER_DSP_H
#define LUPPP_EVENT_HANDLER_DSP_H


// Library
#include <cstring>
#include <iostream>
#include <jack/ringbuffer.h>

// Internal
#include "gui.hxx"
#include "event.hxx"
#include "eventhandler.hxx"

extern Gui* gui;

using namespace std;

void handleGuiEvents()
{
  uint availableRead = jack_ringbuffer_read_space( rbToGui );
  
  while ( availableRead >= sizeof(EventBase) )
  {
    jack_ringbuffer_peek( rbToGui, (char*)processGuiMem, sizeof(EventBase) );
    
    EventBase* e = (EventBase*)processGuiMem;
    
    // recheck the size against the actual event size
    if ( availableRead >= e->size() )
    {
      //cout << "reading event type " << e->type() << endl;
      
      switch ( e->type() )
      {
        case Event::MASTER_VOL: {
          if ( availableRead >= sizeof(EventMasterVol) ) {
            EventMasterVol ev(0);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterVol) );
            //jack->masterVolume = ev.vol;
          } break; }
        case Event::LOAD_SAMPLE: {
          if ( availableRead >= sizeof(EventLoadSample) ) {
            EventLoadSample ev(0);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLoadSample) );
            //jack->addAudioBuffer( ev.audioBufferPtr );
          } break; }
        case Event::PLAY_SAMPLE: {
          if ( availableRead >= sizeof(EventPlaySample) ) {
            EventPlaySample ev(0,0);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventPlaySample) );
            //jack->setPlayBuffer( ev.track, ev.bufferID );
          } break; }
        case Event::METRONOME_ACTIVE: {
          if ( availableRead >= sizeof(EventMetronomeActive) ) {
            EventMetronomeActive ev(false);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMetronomeActive) );
            //jack->getMetronome()->setActive(ev.active);
          } break; }
        case Event::LOOPER_STATE: {
          if ( availableRead >= sizeof(EventLooperState) ) {
            EventLooperState ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperState) );
            //jack->setLooperState( ev.track, ev.state );
          } break; }
        case Event::LOOPER_LOOP_LENGTH: {
          if ( availableRead >= sizeof(EventLooperLoopLength) ) {
            EventLooperLoopLength ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperLoopLength) );
            //jack->setLooperLoopLength( ev.track, ev.scale );
          } break; }
        case Event::LOOPER_PROGRESS: {
          if ( availableRead >= sizeof(EventLooperProgress) ) {
            EventLooperProgress ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperProgress) );
            gui->getTrack(ev.track)->progress.value(ev.progress);
            //jack->setLooperLoopLength( ev.track, ev.scale );
          } break; }
        default:
          {
            cout << "Unkown message!! Will clog ringbuffer" << endl;
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
    availableRead = jack_ringbuffer_read_space( rbToGui );
  }
}

void writeToGuiRingbuffer(EventBase* e)
{
  if ( jack_ringbuffer_write_space(rbToGui) >= e->size() )
  {
    jack_ringbuffer_write( rbToGui, (const char*)e, e->size() );
  }
  else
  {
    cout << "->GUI ringbuffer full!" << endl;
  }
}

#endif // LUPPP_EVENT_HANDLER_DSP_H

