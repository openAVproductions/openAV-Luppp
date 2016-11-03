/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#include "controller/controller.hxx"

#pragma GCC diagnostic ignored "-Wunused-parameter"

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
            LUPPP_NOTE("%s","GUI QUIT");
            gui->quit();
          } break; }
        
        case Event::SAMPLERATE: {
          if ( availableRead >= sizeof(EventSamplerate) ) {
            EventSamplerate ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventSamplerate) );
            gui->samplerate = ev.samplerate;
            //LUPPP_NOTE("Gui Samplerate: %i", gui->samplerate);
          } break; }
        
        /// master
        case Event::MASTER_INPUT_TO: {
          if ( availableRead >= sizeof(EventMasterInputTo) ) {
            EventMasterInputTo ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterInputTo) );
            gui->getMasterTrack()->setInputTo( (int)ev.place, ev.value );
          } break; }
        case Event::MASTER_INPUT_TO_ACTIVE: {
          if ( availableRead >= sizeof(EventMasterInputToActive) ) {
            EventMasterInputToActive ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterInputToActive) );
            gui->getMasterTrack()->setInputToActive( (int)ev.place, ev.active );
          } break; }
        case Event::MASTER_INPUT_VOL: {
          if ( availableRead >= sizeof(EventMasterInputVol) ) {
            EventMasterInputVol ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterInputVol) );
            gui->getMasterTrack()->setInputVol( ev.vol );
          } break; }
        case Event::MASTER_RETURN: {
          if ( availableRead >= sizeof(EventMasterReturn) ) {
            EventMasterReturn ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterReturn) );
            gui->getMasterTrack()->setReturnVol( ev.vol );
          } break; }
        case Event::MASTER_VOL: {
          if ( availableRead >= sizeof(EventMasterVol) ) {
            EventMasterVol ev(0);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMasterVol) );
            gui->getMasterTrack()->getVolume()->fader( ev.vol );
          } break; }
        
        
        case Event::METRONOME_ACTIVE: {
          if ( availableRead >= sizeof(EventMetronomeActive) ) {
            EventMetronomeActive ev(false);
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventMetronomeActive) );
            gui->getMasterTrack()->metronomeEnable(ev.active);
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
        
        
        // FIXME: reset signal level to 0
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
            gui->getTrack(ev.track)->getVolume()->fader( ev.vol );
          } break; }
        
                
        case Event::TRACK_RECORD_ARM: {
          if ( availableRead >= sizeof(EventTrackRecordArm) ) {
            EventTrackRecordArm ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackRecordArm) );
            gui->getTrack(ev.track)->setRecordActive( ev.recordArm );
            break; }
          }
          
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
            // de allocate the AudioBuffer only if reqested
            if(!ev.no_dealloc) {
              gui->getDiskWriter()->writeAudioBuffer( ev.track, ev.scene, ev.ab );
              delete ev.ab;
            } else
            {
              gui->getDiskWriter()->writeAudioBuffer(ev.track, ev.scene, ev.ab,
                                                      gui->saveBufferPath.c_str());
	      gui->saveBufferPath = "";
            }

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
          
        case Event::GRID_SELECT_NEW_CHOSEN: {
          if ( availableRead >= sizeof(EventGridSelectNewChosen) ) {
            EventGridSelectNewChosen ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventGridSelectNewChosen) );
            //LUPPP_NOTE("New special, %i, %i", ev.track, ev.scene);
            for(int i = 0; i < NTRACKS; i++)
            {
              gui->getTrack(i)->getClipSelector()->setSpecial( i == ev.track ? ev.scene : -1 );
            }
            gui->specialTrack = ev.track;
            gui->specialScene = ev.scene;
          } break; }
          
        
        case Event::TRACK_SEND: {
          if ( availableRead >= sizeof(EventTrackSend) ) {
            EventTrackSend ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackSend) );
            if ( ev.send == SEND_POSTFADER )
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->setSend(ev.value );
              }
            if ( ev.send == SEND_XSIDE )
              if (   ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->setXSide( ev.value );
              }
          } break; }
        
        case Event::TRACK_SEND_ACTIVE: {
          if ( availableRead >= sizeof(EventTrackSendActive) ) {
            EventTrackSendActive ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventTrackSendActive) );
            if ( ev.send == SEND_POSTFADER )
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->setSendActive(ev.active );
              }
            if ( ev.send == SEND_KEY )
            {
              if ( ev.track < NTRACKS )
              {
                gui->getTrack(ev.track)->setKeyActive( ev.active );
              }
            }
          } break; }
        
        case Event::GUI_PRINT: {
          if ( availableRead >= sizeof(EventGuiPrint) ) {
            EventGuiPrint ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventGuiPrint) );
            //cout << "DSP: " << ev.getMessage() << endl;
            LUPPP_DSP("%s", ev.getMessage() );
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
              //LUPPP_NOTE("Save buffer sent with t %i, s %i, ab* %i", ev.track, ev.scene, ab );
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
        
        case Event::CONTROLLER_BINDING_ENABLE: {
          if ( availableRead >= sizeof(EventControllerBindingEnable) ) {
            EventControllerBindingEnable ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventControllerBindingEnable) );
            ControllerUI* c = gui->getOptionsWindow()->getControllerUI( ev.controllerID );
            if ( c )
              c->setBindEnable( ev.enable );
            else
              LUPPP_WARN("ControllerUI %i doesn't exist in the UI", ev.controllerID );
          } break; }
        
        case Event::CONTROLLER_BINDING_TARGET: {
          if ( availableRead >= sizeof(EventControllerBindingTarget) ) {
            EventControllerBindingTarget ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventControllerBindingTarget) );
            gui->getOptionsWindow()->setTarget( ev.target );
          } break; }
        
        case Event::CONTROLLER_BINDING_MADE: {
          if ( availableRead >= sizeof(EventControllerBindingMade) ) {
            EventControllerBindingMade ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventControllerBindingMade) );
            ControllerUI* c = gui->getOptionsWindow()->getControllerUI( ev.controllerID );
            if ( c )
              c->addBinding( (Binding*)ev.binding );
            else
              LUPPP_WARN("ControllerUI %i doesn't exist in the UI", ev.controllerID );
          } break; }
        
        case Event::CONTROLLER_INSTANCE_GET_TO_WRITE: {
          if ( availableRead >= sizeof(EventControllerInstanceGetToWrite) ) {
            EventControllerInstanceGetToWrite ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventControllerInstanceGetToWrite) );
            // write the contents of the GenericMIDI controller to .ctlr file
            gui->getDiskWriter()->writeControllerFile( (Controller*)ev.controller );
          } break; }
          
        case Event::CONTROLLER_INSTANCE: {
          if ( availableRead >= sizeof(EventControllerInstance) ) {
            EventControllerInstance ev;
            jack_ringbuffer_read( rbToGui, (char*)&ev, sizeof(EventControllerInstance) );
            // remove this controller from use:
            Controller* c = (Controller*)ev.controller;
            LUPPP_NOTE("Deleting controller %s", c->getName().c_str() );
            // delete will call the destructor for the Controller: this should
            // clean up ports etc, all from the GUI thread as appropriate
            delete c;
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

