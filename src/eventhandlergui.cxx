
#ifndef LUPPP_EVENT_HANDLER_DSP_H
#define LUPPP_EVENT_HANDLER_DSP_H


// Library
#include <cstring>
#include <iostream>
#include <jack/ringbuffer.h>

// Internal
#include "gui.hxx"
#include "event.hxx"
#include "audiobuffer.hxx"
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
            //gui->getTrack(ev.track)->getClipSelector()->setState(ev.scene, ev.state);
            //jack->setLooperState( ev.track, ev.state );
          } break; }
        case Event::LOOPER_LOOP_LENGTH: {
          if ( availableRead >= sizeof(EventLooperLoopLength) ) {
            //EventLooperLoopLength ev;
            //jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperLoopLength) );
            //jack->setLooperLoopLength( ev.track, ev.scale );
          } break; }
        case Event::LOOPER_PROGRESS: {
          if ( availableRead >= sizeof(EventLooperProgress) ) {
            EventLooperProgress ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperProgress) );
            
            // only update if significant change
            //if ( ev.progress - gui->getTrack(ev.track)->radial.value() >= 0.05 ||
            //     ev.progress > 0.9 )
              gui->getTrack(ev.track)->radial.value(ev.progress);
            
          } break; }
        
        case Event::TRACK_SIGNAL_LEVEL: {
          if ( availableRead >= sizeof(EventTrackSignalLevel) ) {
            EventTrackSignalLevel ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackSignalLevel) );
            if ( ev.track < 0 ) {
              //printf("%f\t%f\n", ev.left, ev.right);
              gui->getMasterTrack()->getVolume()->amplitude( ev.left, ev.right );
            } else {
              gui->getTrack(ev.track)->getVolume()->amplitude( ev.left, ev.right ); }
          } break; }
        case Event::TRACK_VOLUME: {
          if ( availableRead >= sizeof(EventTrackVol) ) {
            EventTrackVol ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackVol) );
            if ( ev.track < 0 )
            {
              gui->getMasterTrack()->getVolume()->value( ev.vol );
            }
            else
              gui->getTrack(ev.track)->getVolume()->value( ev.vol );
          } break; }
        
        
        case Event::GRID_STATE: {
          if ( availableRead >= sizeof(EventGridState) ) {
            EventGridState ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventGridState) );
            gui->getTrack(ev.track)->getClipSelector()->setState( ev.scene, ev.state );
            if ( ev.state == GridLogic::STATE_RECORDING )
              gui->getTrack(ev.track)->getRadialStatus()->recording( true );
            else
              gui->getTrack(ev.track)->getRadialStatus()->recording( false );
          } break; }
        
        case Event::TRACK_SEND: {
          if ( availableRead >= sizeof(EventTrackSend) ) {
            EventTrackSend ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackSend) );
            if ( ev.send == SEND_REV )
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->rev.value( ev.value );
              }
            if ( ev.send == SEND_POST )
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->post.value( ev.value );
              }
            if ( ev.send == SEND_SIDE )
            {
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->side.value( ev.value );
              }
            }
          } break; }
        
        case Event::GUI_PRINT: {
          if ( availableRead >= sizeof(EventGuiPrint) ) {
            EventGuiPrint ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventGuiPrint) );
            cout << "DSP: " << ev.getMessage() << endl;
          } break; }
        case Event::TIME_BAR_BEAT: {
          if ( availableRead >= sizeof(EventTimeBarBeat) ) {
            EventTimeBarBeat ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTimeBarBeat) );
            gui->getMasterTrack()->setBarBeat( ev.bar, ev.beat);
          } break; }
        case Event::TIME_TEMPO_TAP: {
          if ( availableRead >= sizeof(EventTimeTempoTap) ) {
            EventTimeTempoTap ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTimeTempoTap) );
            gui->getMasterTrack()->setTapTempo( ev.pressed );
          } break; }
        
        
        case Event::LOOPER_REQUEST_BUFFER: {
          if ( availableRead >= sizeof(EventLooperClipRequestBuffer) ) {
            EventLooperClipRequestBuffer ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventLooperClipRequestBuffer) );
            
            /// allocate a new AudioBuffer with ev.numElements, pass back to DSP
            AudioBuffer* ab = new AudioBuffer(ev.numElements);
            EventLooperClipRequestBuffer returnEvent(ev.track, ev.scene, ab);
            writeToDspRingbuffer( &returnEvent );
#ifdef DEBUG_BUFFER
            printf("new buffer going to track %i, scene %i\n",ev.track, ev.scene);
#endif
          } break; }
        case Event::DEALLOCATE_BUFFER: {
          if ( availableRead >= sizeof(EventDeallocateBuffer) ) {
            EventDeallocateBuffer ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventDeallocateBuffer) );
            delete ev.ab;
          } break; }
        
        
        default:
          {
            cout << "GUI: Unkown message!! Will clog ringbuffer" << endl;
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

