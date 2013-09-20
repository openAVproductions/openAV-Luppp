
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
        case Event::QUIT: {
          if ( availableRead >= sizeof(EventQuit) ) {
            EventQuit ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventQuit) );
            gui->quit();
            LUPPP_NOTE("%s","GUI QUIT");
          } break; }
        
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
            if ( ev.track == -2 ) {
              gui->getMasterTrack()->getInputVolume()->amplitude( ev.left, ev.right );
            } else if ( ev.track == -1 ) {
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
              gui->getMasterTrack()->getVolume()->fader( ev.vol );
            }
            else
              gui->getTrack(ev.track)->getVolume()->fader( ev.vol );
          } break; }
          
          
        case Event::TIME_BPM: {
          if ( availableRead >= sizeof(EventTimeBPM) ) {
            EventTimeBPM ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTimeBPM) );
            gui->getMasterTrack()->setBpm( ev.bpm );
          } break; }
        
        
        case Event::STATE_SAVE_BUFFER: {
          if ( availableRead >= sizeof(EventStateSaveBuffer) ) {
            EventStateSaveBuffer ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventStateSaveBuffer) );
#ifdef DEBUG_SAVE
            cout << "EventSaveBuffer: " << ev.track << " " << ev.scene << " " << ev.ab->getID() << endl;
#endif
            gui->getDiskWriter()->writeAudioBuffer( ev.track, ev.scene, ev.ab );
            // de allocate the AudioBuffer
            delete ev.ab;
          } break; }
          
        case Event::STATE_SAVE_FINISH: {
          if ( availableRead >= sizeof(EventStateSaveFinish) ) {
            EventStateSaveFinish ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventStateSaveFinish) );
#ifdef DEBUG_SAVE
            cout << "EventSaveFinish!" << endl;
#endif
            gui->getDiskWriter()->writeSession();
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
        case Event::GRID_LAUNCH_SCENE: {
          if ( availableRead >= sizeof(EventGridLaunchScene) ) {
            EventGridLaunchScene ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventGridLaunchScene) );
            for(int i = 0; i < NSCENES; i++)
              gui->getMasterTrack()->getClipSelector()->setState( i, GridLogic::STATE_EMPTY );
            gui->getMasterTrack()->getClipSelector()->setState( ev.scene, GridLogic::STATE_PLAYING );
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
        
        case Event::REQUEST_SAVE_BUFFER: {
          if ( availableRead >= sizeof(EventRequestSaveBuffer) ) {
            EventRequestSaveBuffer ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventRequestSaveBuffer) );
#ifdef DEBUG_BUFFER
            printf("Save buffer to track %i, scene %i\n",ev.track, ev.scene);
#endif
            /// allocate a new AudioBuffer with ev.numElements, pass back to DSP
            AudioBuffer* ab = new AudioBuffer(ev.bufferSize);
            
            if ( ab )
            {
              cout << "Save buffer sent with t s ab* " << ev.track << " " << ev.scene << " " << ab << endl;
              EventRequestSaveBuffer returnEvent( ev.track, ev.scene, ab);
              writeToDspRingbuffer( &returnEvent );
            }
            else
            {
              cout << "error allocating save buffer!" << endl;
            }
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

