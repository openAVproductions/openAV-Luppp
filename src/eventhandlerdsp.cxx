
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

#include "logic.hxx"

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
        case Event::STATE_SAVE: {
          if ( availableRead >= sizeof(EventStateSave) ) {
            EventStateSave ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventStateSave) );
            jack->getState()->save();
          } break; }
        case Event::STATE_RESET: {
          if ( availableRead >= sizeof(EventStateReset) ) {
            EventStateReset ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventStateReset) );
            jack->getState()->reset();
          } break; }
        
        // ========= MASTER ===
        case Event::MASTER_VOL: {
          if ( availableRead >= sizeof(EventMasterVol) ) {
            EventMasterVol ev(0);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMasterVol) );
            jack->masterVolume( ev.vol );
          } break; }
        case Event::MASTER_RETURN: {
          if ( availableRead >= sizeof(EventMasterReturn) ) {
            EventMasterReturn ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMasterReturn) );
            //jack->getLogic()->trackSend( ev.track, ev.send, ev.value );
          } break; }
        case Event::MASTER_INPUT_TO: {
          if ( availableRead >= sizeof(EventMasterInputTo) ) {
            EventMasterInputTo ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMasterInputTo) );
            jack->getLogic()->masterInputTo( ev.place, ev.value );
          } break; }
        
        // ========= GRID =====
        case Event::GRID_EVENT: {
          if ( availableRead >= sizeof(EventGridEvent) ) {
            EventGridEvent ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventGridEvent) );
            if ( ev.pressed )
              jack->getGridLogic()->pressed( ev.track, ev.scene );
            else
              jack->getGridLogic()->released( ev.track, ev.scene );
          } break; }
        
        case Event::GRID_LAUNCH_SCENE: {
            EventGridLaunchScene ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventGridLaunchScene) );
            jack->getGridLogic()->launchScene( ev.scene );
            break; }
        
        case Event::LOOPER_LOAD: {
          if ( availableRead >= sizeof(EventLooperLoad) ) {
            EventLooperLoad ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperLoad) );
            jack->getGridLogic()->load( ev.track, ev.clip, (AudioBuffer*)ev.audioBuffer );
          } break; }
        case Event::METRONOME_ACTIVE: {
          if ( availableRead >= sizeof(EventMetronomeActive) ) {
            EventMetronomeActive ev(false);
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventMetronomeActive) );
            jack->getLogic()->metronomeEnable(ev.active);
          } break; }
        case Event::LOOPER_STATE: {
          if ( availableRead >= sizeof(EventLooperState) ) {
            EventLooperState ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperState) );
            //jack->setLooperState( ev.track, ev.scene, ev.state );
          } break; }
        case Event::LOOPER_LOOP_LENGTH: {
          if ( availableRead >= sizeof(EventLooperLoopLength) ) {
            EventLooperLoopLength ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperLoopLength) );
            jack->getLogic()->looperClipLenght( ev.track, ev.scene, ev.beats );
          } break; }
          
          
        case Event::LOOPER_LOOP_USE_AS_TEMPO: {
          if ( availableRead >= sizeof(EventLooperUseAsTempo) ) {
            EventLooperUseAsTempo ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperUseAsTempo) );
            jack->getLogic()->looperUseAsTempo( ev.track, ev.scene );
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
        
        // ======== FX ===========
        
        case Event::FX_REVERB: break;
        /*{
          if ( availableRead >= sizeof(EventFxReverb) ) {
            EventFxReverb ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventFxReverb) );
            // TODO implement reverb
            break; }
          }
        */
        case Event::TRACK_VOLUME: {
          if ( availableRead >= sizeof(EventTrackVol) ) {
            EventTrackVol ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventTrackVol) );
            jack->getLogic()->trackVolume( ev.track, ev.vol );
            break; }
          }
          
        case Event::TRACK_SEND: {
          if ( availableRead >= sizeof(EventTrackSend) ) {
            EventTrackSend ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventTrackSend) );
            jack->getLogic()->trackSend( ev.track, ev.send, ev.value );
          } break; }
          
        
        // ========= LUPPP INTERNAL =====
        case Event::LOOPER_REQUEST_BUFFER: {
          if ( availableRead >= sizeof(EventLooperClipRequestBuffer) ) {
            EventLooperClipRequestBuffer ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventLooperClipRequestBuffer) );
            jack->getLooper( ev.track )->setRequestedBuffer( ev.scene, ev.ab );
          } break; }
        
        case Event::REQUEST_SAVE_BUFFER: {
          if ( availableRead >= sizeof(EventRequestSaveBuffer) ) {
            EventRequestSaveBuffer ev;
            jack_ringbuffer_read( rbToDsp, (char*)&ev, sizeof(EventRequestSaveBuffer) );
            cout << "Save buffer sent with t s ab* " << ev.track << " " << ev.scene << " " << ev.ab << endl;
            jack->getLooper( ev.track )->getClip(ev.scene)->recieveSaveBuffer( ev.ab );
          } break; }
        
        default:
          {
            cout << "DSP: Unkown message!! Will clog ringbuffer" << endl;
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

