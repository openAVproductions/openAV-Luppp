
#include "jackclient.hpp"

#include <cmath>
#include <iostream>

#include "effectstate.hpp"
#include "audiobuffer.hpp"

#include "top.hpp"

using namespace std;

JackClient::JackClient( Top* t) :
                        time(t),
                        mixer(t)
{
  top = t;
  
  client = jack_client_open ( "Luppp", JackNullOption , 0 , 0 );
  
  t->bufferSize = jack_get_buffer_size(client);
  t->samplerate = jack_get_sample_rate(client);
  
  t->initialize();
  
  inputPort = jack_port_register ( client,
                                    "input",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput,
                                    0 );
  
  outputPortW = jack_port_register ( client,
                                    "output_w",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput,
                                    0 );
  
  outputPortX = jack_port_register ( client,
                                    "output_x",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput,
                                    0 );
  
  outputPortY = jack_port_register ( client,
                                    "output_y",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput,
                                    0 );
  
  outputPortZ = jack_port_register ( client,
                                    "output_z",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput,
                                    0 );
  
  midiInputPort = jack_port_register ( client,
                                    "midi_input",
                                    JACK_DEFAULT_MIDI_TYPE,
                                    JackPortIsInput,
                                    0 );
  
  
  trackControlMode = -1;
  apcInputPort = jack_port_register ( client,
                                    "apc_input",
                                    JACK_DEFAULT_MIDI_TYPE,
                                    JackPortIsInput,
                                    0 );
  
  apcOutputPort = jack_port_register ( client,
                                    "apc_output",
                                    JACK_DEFAULT_MIDI_TYPE,
                                    JackPortIsOutput,
                                    0 );
  
  
  //jack_set_process_callback  (client, process , 0);
  
  int retval = jack_set_process_callback( client,
                                  static_process,
                                  static_cast<void*>(this));
  
  if(retval) {
     cerr << "JackClient::JackClient() Could not set process callback." << endl;
     return;
  }
  
  jack_activate(client);
}

int JackClient::static_process(jack_nframes_t nframes, void* thisPointer)
{
  return static_cast<JackClient*>(thisPointer)->process(nframes);
}

int JackClient::processRtQueue()
{
  bool moreEventsWaiting = true;
  
  // loop over events queue, return when no events to process
  while ( moreEventsWaiting )
  {
    EngineEvent* e = top->toEngineQueue.pull();
    
    if ( e == 0 )
    {
      // exit the loop
      break;
    }
    
    //std::cout << "processRtQueue() in engine got event!" << std::endl;
    
    if ( e->type == EE_MIXER_VOLUME )
    {
      top->state.setVolume( e->ia, e->fa );
    }
    else if ( e->type == EE_TRACK_SET_MUTE ) {
      top->state.setMute( e->ia, e->ib );
    }
    else if ( e->type == EE_TRACK_SET_SOLO ) {
      top->state.setSolo( e->ia, e->ib );
    }
    else if ( e->type == EE_TRACK_SET_REC ) {
      cout << "JC:pRTq() SET_REC event" << endl;
      top->state.setRec( e->ia, e->ib );
    }
    else if ( e->type == EE_TRACK_SET_PAN ) {
      top->state.setPan( e->ia, e->ib );
    }
    else if ( e->type == EE_STATE_SET_AUDIO_BUFFER ) {
      // sets the unique bufferID into the AudioBuffer list, so we can
      // retrieve the actual AudioBuffer instance with the unique id
      // with a call to   state->getAudioBuffer( bufID );
      cout << "STATE_SET_AUDIO_BUFFER, buffer id = " << flush;
      AudioBuffer* buffer = (AudioBuffer*)e->vPtr;
      cout << buffer->getID() << endl;
      top->state.setAudioBuffer( buffer );
    }
    else if ( e->type == EE_STATE_NEW_EFFECT ) {
      // we get an Effect*, which we need to insert track @ e->ia, pos e->ib
      //cout << "JackClient::processRtQueue() STATE_NEW_EFFECT, ID = " << flush;
      Effect* effect = (Effect*)e->vPtr;
      //cout << effect->getID() << endl;
      
      int ret = mixer.addEffect(e->ia, e->ib, effect);
      if ( ret == 0 ) // success
      {
        //cout << "JackClient::processRtQ() Sending GUI NEW EFFECT event! track = " << e->ia << endl;
        EngineEvent* x = top->toEngineEmptyEventQueue.pull();
        x->setStateEffect(e->ia, e->ib, (int)effect->getType(), 0 ); // vPtr = 0, don't give GUI access!
        top->toGuiQueue.push(x);
      }
    }
    else if ( e->type == EE_LOOPER_LOAD ) {
      // insert the correct buffer ID into the list, so we can retrieve by sceneID later
      cout << "JackClient::processRtQueue() EE_LOOPER_LOAD " << e->ia << ", " << e->ib << "," << e->ic << endl;
      top->state.setClipSelectorState( e->ia, e->ib, e->ic );
    }
    else if ( e->type == EE_LOOPER_SELECT_BUFFER) {
      // tells the AudioSource @ track ia to play scene ib
      
      // TIME HACK
      cout << "EE_LOOPER_SELECT_BUFFER " << e->ia << ", " << e->ib << endl;
      time.processEngineEvent( e );
      
      //top->state.clipSelectorQueue(e->ia, e->ib);
    }
    else if ( e->type == EE_TRACK_SET_PLUGIN_PARAMETER) {
      // send all values on to stateStore, from there we message UI, and
      // we do the logic on which EffectID we need to write to
      //top->state.setPluginParameter(e->ia,e->ib,e->ic,e->fa);
    }
    else if ( e->type == EE_ADD_TRACK) {
      // GUI / Controller can both add tracks, so we get the ADD_TRACK event here
      // then bounce it to GUI thread, it creates the nessisary instances non-RT,
      // and then it bounces the nessisary pointers back to Engine (see EE_ADD_TRACK_POINTERS)
      //cout << "JackC::processRtQ Got Add track event, bouncing to GUI!" << endl;
      EngineEvent* x = top->toEngineEmptyEventQueue.pull();
      
      // tell GUI the ID for the new track
      x->addTrack( top->state.getNumTracks() + 1 );
      top->toGuiQueue.push(x);
    }
    else if ( e->type == EE_ADD_TRACK_POINTERS) {
      //cout << "JackC::processRtQ Got ADD TRACK POINTERS event, setting in Mixer & State!" << endl;
      
      // vPtr is the AudioTrack*, so cast & send to mixer to insert
      mixer.addTrack( (AudioTrack*) e->vPtr );
      
      // vPtr 2, 3 & 4 are BAS, Clip, TrackOutput in that order.
      top->state.addTrack( (BufferAudioSourceState*) e->vPtr2, (ClipSelectorState*)e->vPtr3, (TrackOutputState*)e->vPtr4 );
    }
    
  }
  
  return 0;
}

int JackClient::process(jack_nframes_t nframes)
{
  this->nframes = nframes;
  portBufferList.inputAudio = (float*)jack_port_get_buffer ( inputPort, nframes);
  
  portBufferList.outputW = (float*)jack_port_get_buffer (outputPortW, nframes);
  portBufferList.outputX = (float*)jack_port_get_buffer (outputPortX, nframes);
  portBufferList.outputY = (float*)jack_port_get_buffer (outputPortY, nframes);
  portBufferList.outputZ = (float*)jack_port_get_buffer (outputPortZ, nframes);
  
  // class variable for buffer
  apcOutputBuffer = jack_port_get_buffer ( apcOutputPort, nframes);
  jack_midi_clear_buffer(apcOutputBuffer);
  
  processRtQueue();
  
  apcRead(nframes);
  
  time.process( jack_get_current_transport_frame(client) );
  
  // handle incoming midi
  processMidi(nframes);
  
  mixer.process(nframes, recordInput, portBufferList);
  
  return true;
};

int JackClient::processMidi(jack_nframes_t nframes)
{
  // process incoming MIDI
  midiInputBuffer = jack_port_get_buffer (midiInputPort, nframes);
  jack_midi_event_t in_event; // event buffer
  
  int midiIndex = 0;
  int event_count = (int) jack_midi_get_event_count(midiInputBuffer); // numMessages
  
  // set to invalid, if b1 = anything its a new valid message
  top->state.midib1 = -1;
  
  while (midiIndex < event_count)
  {
    jack_midi_event_get(&in_event, midiInputBuffer, midiIndex); // get first message
    
    // buffer[0] == type
    // buffer[1] == message & channel
    // buffer[2] == data
    int b1 = (int)in_event.buffer[0];
    int b2 = (int)in_event.buffer[1];
    int b3 = (int)in_event.buffer[2];
    
    
    top->state.midib1 = b1;
    top->state.midib2 = b2;
    top->state.midib3 = b3;
    
    cout << "JACK MIDI IN" << b1 << "  " << b2 << "  " << b3 << endl;
    
    if ( b1 >= 144 && b1 < 144 + 16 ) // note on
    {
      /*
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      std::advance(i,1);
      
      // frequency of cycles trough the buffer
      float freq = pow( 2.0, ((double)b2 - 69.0) / 12.0 );
      
      // buffer size = 512
      float cyclesTroughBuffer = freq;
      
      std::cout << "CyclesTroughBuffer = " << cyclesTroughBuffer << std::endl;
      
      i->speed = freq;
      
      std::cout << "i->speed = (freq) " << i->speed << endl;
      */
    }
    else if ( b1 >= 128 && b1 < 128 + 16 ) // note off
    {
      /*
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      std::advance(i,1);
      i->speed = 0;
      std::cout << "i->speed = " << i->speed << endl;
      */
    }
    else if ( b1 >= 176 && b1 < 176 + 16 ) // CC change Device control
    {
      /*
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      if ( b2 == 17 )
        std::advance(i,1);
      float value = (b3/127.f);
      i->speed = value  + 0.5;
      std::cout << "i->speed = " << i->speed << endl;
      
      EngineEvent* x = top->toEngineEmptyEventQueue.pull();
      x->setTrackSpeed(0, value);
      top->toGuiQueue.push(x);
      top->guiDispatcher->emit();
      */
    }
    
    
    midiIndex++;
  }
  
  return 0;
}

void JackClient::deactivate()
{
  jack_deactivate(client);
  jack_client_close (client);
  std::cout << "Jack::Deactivate()" << std::endl;
}

void JackClient::writeMidi(void* portBuffer, int b1, int b2, int b3)
{
  unsigned char* buffer = jack_midi_event_reserve( portBuffer, 0, 3);
  if( buffer == 0 )
  {
    std::cout << "Error: APC writeMidi() write buffer == 0" << std::endl;
    return;
  }
  else
  {
    cout << "JC::writeMidi() " << b1 << ", " << b2 << ", " << b3 << endl; 
    buffer[0] = b1;
    buffer[1] = b2;
    buffer[2] = b3;
  }
}

// this function will update the parameters on the APC in the DEVICE CONTROL
// section. It should be called when we move between Effects on the same track
void JackClient::apcSendDeviceControl(int track, int device, void* apcOutputBuffer)
{
  // get UID of state
  int uid = mixer.getEffectID(track, device);
  if ( uid == -1 )
    return;
  
  // retrieve state
  EffectState* state = top->state.getEffectState(uid);
  
  // write each parameter value
  for ( int i = 0; i < 8; i++ )
  {
    writeMidi(apcOutputBuffer, 176 + track, 16 + i, state->values[i] * 127 );
  }
  
  int onOff = 128 + track;
  if ( state->active )
    onOff = 144 + track;
  
  writeMidi(apcOutputBuffer, onOff, 59, 127); // write device On/Off LED
  
}

void* JackClient::getApcOutputBuffer()
{
  return apcOutputBuffer;
}

void JackClient::apcRead( int nframes )
{
  void* apcInBuffer = jack_port_get_buffer ( apcInputPort, nframes);
  
  jack_midi_event_t in_event;
  
  int index = 0;
  int event_count = (int) jack_midi_get_event_count(apcInBuffer); // numMessages
  
  while (index < event_count)
  {
    jack_midi_event_get(&in_event, apcInBuffer, index); // get first message
    
    int b1 = (int)in_event.buffer[0];
    int b2 = (int)in_event.buffer[1];
    int b3 = (int)in_event.buffer[2];
    
    //std::cout << "From APC  B1: " << b1 << "  B2: " << b2 << "  B3: " << b3 << std::endl;
    
    /* #### Track selection
    if ( b1 >= 176 && b1 < 176 + 16 )
    {
      if (  ccHistory[0] == 16 &&
            ccHistory[1] == 17 &&
            ccHistory[2] == 18 &&
            ccHistory[3] == 19 &&
            ccHistory[4] == 20 &&
            ccHistory[5] == 21 &&
            ccHistory[6] == 22 &&
            ccHistory[7] == 23 )
      {
        // we have a "track selection" button press, so do a "track select" OSC message, -1 means "return to prev selected on track"
        int id = b1 - 176;
        if (id == 8 ) // master
          id = -1;
        std::cout << "TRACK SELECTION PRESSED!  ID:" << id << std::endl;
        ////lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/selectdevice", "ii", id, -1);
        //ccHistory[7] = -1; // wreck the history, to make sure we need another "full" change before resending
      }
      else
      {
        // move all elements over 1 in the history
        ccHistory[0] = ccHistory[1];
        ccHistory[1] = ccHistory[2];
        ccHistory[2] = ccHistory[3];
        ccHistory[3] = ccHistory[4];
        ccHistory[4] = ccHistory[5];
        ccHistory[5] = ccHistory[6];
        ccHistory[6] = ccHistory[7];
        ccHistory[7] = b2;
      }
    }
    */
    
    /*
    if ( b1 == 144 && b2 == 98 ) // shift button, switch mode!
    {
      std::string x;
      
      if ( apcMode == APC_MODE_CLIP ) {
        apcMode = APC_MODE_SAMPLER;
        x = "SAMPLER";
      }
      else if ( apcMode == APC_MODE_SAMPLER ) {
        apcMode = APC_MODE_CLIP;
        x = "CLIP";
      }
      
      std::cout << "SHIFT! Switching to " << x << " mode now!" << std::endl;
    }
    */
    
    
    // apc 40 Device Control On / Off
    if( b2 == 59 )
    {
      bool active = false;
      if ( b1 >= 144 && b1 < 144 + 16 ) // on
      {
        active = true;
      }
      // get unique ID of current selected track & device, then setPluginParam that UID
      int uid = mixer.getEffectID(top->controller->getTrack(), top->controller->getDevice());
      top->state.setPluginActive(uid, active);
    }
    
    // APc <- Selected device button
    if( b2 == 60 && ( b1 >= 144 && b1 < 144 + 16 ) )
    {
      int trackID = b1 - 144;
      int device = top->controller->getDevice();
      if ( device > 0 ) // range check
        top->controller->setTrackDevice(trackID, --device);
      std::cout << "APC <- Device button: NOTE ON track " << trackID << " device " << device << std::endl;
      writeMidi( apcOutputBuffer, 128 + trackID, 60, 127 ); // turn off LED again, so next press is ON
      
      apcSendDeviceControl(trackID, device, apcOutputBuffer);
    }
    
    // Apc -> selected device button
    if( b2 == 61 && ( b1 >= 144 && b1 < 144 + 16 ) )
    {
      int trackID = b1 - 144;
      int device = top->controller->getDevice();
      if ( device < 9 )
        top->controller->setTrackDevice(trackID,++device);
      std::cout << "APC -> Device button: NOTE ON track = " << trackID << " device " << device << std::endl;
      writeMidi( apcOutputBuffer, 128 + trackID, 61, 127 ); // turn off LED again, so next press is ON
      
      apcSendDeviceControl(trackID, device, apcOutputBuffer);
    }
    
    // apc 40 master fader
    if ( (int)in_event.buffer[0] == 176 && (int)in_event.buffer[1] == 14 )
    {
      top->state.beatSmash = ((b3/127.f) * 10);
      if ( top->state.beatSmash < 3 )
        top->state.beatSmash = false;
      else
        top->state.beatSmash -= 3;
      
      std::cout << "BeatSmash = " << top->state.beatSmash  << std::endl;
    }
    // apc 40 rest of the faders
    if ( (int)in_event.buffer[1] == 7 )
    {
      int trackID = (int)in_event.buffer[0] - 176;
      //std::cout << " SETTING TRACK FADER " << trackID << " FROM APC40!" << std::endl;
      
      /*
      std::list<TrackOutputState>::iterator i = top->state.trackoutputState.begin();
      std::advance(i,trackID);
      
      i->volume = value  + 0.5;
      //std::cout << "i->volume = " << i->volume << endl;
      */
      
      float value = (b3/127.f);
      top->state.setVolume( trackID, value );
    }
    
    
    // apc 40 track control mode buttons
    if ( b1 == 144 && (b2 >= 87 && b2 < 91) )
    {
      std::cout << "Apc Track Control Mode = " << b2 << "\t";
      if ( b2 == APC_TRACK_CONTROL_MODE_PAN)   { std::cout << "PAN" << std::endl; }
      if ( b2 == APC_TRACK_CONTROL_MODE_SEND_A){ std::cout << "A" << std::endl; }
      if ( b2 == APC_TRACK_CONTROL_MODE_SEND_B){ std::cout << "B" << std::endl; }
      if ( b2 == APC_TRACK_CONTROL_MODE_SEND_C){ std::cout << "C" << std::endl; }
      
      std::cout  << "setTrackControlMode()  off = " << trackControlMode << "  on = " << b2 << std::endl;
      writeMidi( apcOutputBuffer, 128, trackControlMode, 0); // turn off previous mode
      trackControlMode = (ApcTrackControlMode)b2;
      
      // turn on new mode
      writeMidi( apcOutputBuffer, 144, trackControlMode, 127);
      
      
    }
    
    // apc 40 top controllers (bank of 8)
    if ( b1 == 176 && b2 >= 48 && b2 < 56 )
    {
      int trackID;
      float value;
      
      // for send A
      ClipSelectorState* clipSelectorState;
      std::list<ClipInfo>::iterator clipIter;
      int playingScene;
      
      switch ( trackControlMode )
      {
        case APC_TRACK_CONTROL_MODE_PAN:
          trackID = b2 - 48;
          value = (b3 / 63.f) - 1.f;
          std::cout << "Track Control: PAN MODE:  trackID: " << trackID << "  value " << value << std::endl;
          top->state.setPan( trackID, value );
          break;
        case APC_TRACK_CONTROL_MODE_SEND_A:
          trackID = b2 - 48;
          value = b3 / 127.f;
          std::cout << "Track Control: SEND A:  trackID: " << trackID << "  value " << value << std::endl;
          
          // get clipSelectorState for current track
          clipSelectorState = top->state.getClipSelectorState(trackID);
          playingScene = clipSelectorState->playing;
          cout << " clipSelectorState->playing " << playingScene << endl;
          
          if ( playingScene < 0 )
            return;
          
          clipIter = clipSelectorState->clipInfo.begin();
          std::advance(clipIter, playingScene);
          clipIter->speed = 0.5 + value;
          
          break;
        case APC_TRACK_CONTROL_MODE_SEND_B:
          trackID = b2 - 48;
          value = (b3 / 63.f) - 1.f;
          std::cout << "Track Control: SEND B:  trackID: " << -2 << "  value " << value << std::endl;
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, 2, b2 - 47, b3 / 127.f );
          break;
      }
      /*
      if ( b2 == 48 ) // knob 1
      {
        //std::cout << " SETTING CONTROLLER BANK 1 FROM APC40!" << std::endl;
        //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, 1, b2 - 48, b3 / 127.f );
      }
      */
    }
    
    // apc 40 grid - PRESS events
    if ( b1 >= 144 && b1 < 144 + 16 )
    {
      if ( b2 >= 52 && b2 < 58 ) // the 5 vertial "launch" buttons, AND the clip stop (#52)
      {
        int id = b1 - 144;
        int val= b2 - 53; // here we take 53 away, so the clip stop button will result in -1 for buffer ID
        
        std::cout << "SETTING BUFFER FROM APC40! ID:" << id << " val: " << val << std::endl;
        
        // REACT!! Make engine playback the CLIP!
        EngineEvent* x = top->toEngineEmptyEventQueue.pull();
        x->looperSelectBuffer(id,val);
        time.processEngineEvent(x);
      }
    }
    
    // APC 40 grid RELEASE events
    if ( b1 >= 128 && b1 < 128 + 16 )
    {
      if ( b2 >= 52 && b2 < 58 ) // the 5 vertial "launch" buttons, AND the clip stop (#52)
      {
        // UPDATE!! Make the APC show the right colour LED now that the release has occured!
        int track = b1 - 128;
        int block = b2 - 53;
        
        if ( block >= 0 )
        {
          ClipSelectorState* clipSelectorState = top->state.getClipSelectorState(track);
          std::list<ClipInfo>::iterator clipState =  clipSelectorState->clipInfo.begin();
          std::advance(clipState, block);
          
          if ( clipState->state == CLIP_STATE_RECORDING )
            writeMidi( apcOutputBuffer, 144 + track, b2, 3 );
          else
            writeMidi( apcOutputBuffer, 144 + track, b2, 1 );
        }
      }
    }
    
    // apc 40 lower controllers (bank of 8)
    if ( b1 >= 176 && b1 < 176 + 16 )
    {
      if ( b2 >= 16 && b2 < 24 && b1 != 184 ) // all controllers, excluding master track
      {
        int track = b1 - 176;
        int device= top->controller->getDevice();
        top->controller->setTrackDevice(track, device);
        std::cout << "APC: Device Control on track " << track << " device " << device << " param " << b2 -16 << std::endl;
        
        // get unique ID of current selected track, then setPluginParam that UID
        int uid = mixer.getEffectID(track, device);
        
        EngineEvent* x = top->toEngineEmptyEventQueue.pull();
        x->setPluginParameter(uid,
                              -1,
                              b2-16,
                              b3 / 127.);
        time.processEngineEvent(x);
        
        /*
        // here we change the OSC param we send based on the track & device selection
        if ( trackID < trackStateVector.size() )
        {
          int selDev = trackStateVector.at(trackID)->selectedDevice;
          std::cout << "Track ID " << trackID << " selDev = " << selDev << " param = " << b2 - 16 << " value " << b3 / 127.f << std::endl;
          
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", trackID, selDev, b2 - 16, b3 / 127.f );
        }
        */
      }
      
      if ( b1 == 184 ) // master track
      {
        if ( b2 == 16 ) // pure data Delay wet
        {
          //((Mixer*)(rh->mixer))->setPluginParameter( -2, -1, 2, b3 / 127.f); // delay dry/wet
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, -1, 2, b3 / 127.f );
        }
        if ( b2 == 19 ) // pure data LPF
        {
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, -1, 0, b3 / 127.f );
        }
        if ( b2 == 20 ) // pure data Delay time
        {
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, -1, 3, b3 / 127.f );
        }
        if ( b2 == 23 ) // pure data HPF
        {
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, -1, 1, b3 / 127.f );
        }
      }
    }
    
    // apc 40 scene launch & Stop All Clips buttons
    if ( b1 == 144 && ( b2 >= 81 && b2 < 87 ) )
    {
      int sceneID = b2 - 82;
      std::cout << "APC: Scene Launch on Scene " << sceneID << std::endl;
      
      for (int i = 0; i < 8; i++ )
      {
        //lo_send( //lo_address_new(NULL, "14688") , "/luppp/looper/selectbuffer", "ii", i, sceneID );
      }
    }
    
    if ( b2 == 48 )
    {
      if ( b1 >= 144 && b1 < 144 + 16 ) // REC on
      {
        int trackID = b1 - 144;
        std::cout << "APC: REC on track " << trackID << " on!" << std::endl;
        
        top->state.setRec( trackID, true );
      }
      else if ( b1 >= 128 && b1 < 128 + 16 ) // rec off
      {
        int trackID = b1 - 128;
        std::cout << "APC: REC on track " << trackID << " off!" << std::endl;
        
        top->state.setRec( trackID, false );
      }
    }
    if ( b2 == 49 )
    {
      if ( b1 >= 144 && b1 < 144 + 16 ) // Solo on
      {
        int trackID = b1 - 144;
        std::cout << "APC: SOLO on track " << trackID << " on!" << std::endl;
        //lo_send( //lo_address_new(NULL, "14688") , "/luppp/track/solo", "ii", trackID, 1 );
      }
      else if ( b1 >= 128 && b1 < 128 + 16 ) // solo off
      {
        int trackID = b1 - 128;
        std::cout << "APC: SOLO on track " << trackID << " off!" << std::endl;
        //lo_send( //lo_address_new(NULL, "14688") , "/luppp/track/solo", "ii", trackID, 0 );
      }
    }
    if ( b2 == 50 )
    {
      // LED ON = Active = NOT MUTED
      bool onOff = true;
      int trackID = b1 - 128;
      if ( b1 >= 144 && b1 < 144 + 16 )
      {
        onOff = false;
        trackID = b1 - 144;
      }
      //std::cout << "APC: Mute on track " << trackID << " off!" << std::endl;
      if ( top->state.setMute( trackID, onOff ) == 0 )
      {
        EngineEvent* x = top->toEngineEmptyEventQueue.pull();
        x->setTrackMute(trackID, onOff);
        top->toGuiQueue.push(x);
        top->guiDispatcher->emit();
      }
      else
      {
        //cout << "MUTE OUT OF BOUNDS!" << endl;
      }
    }
    
    if ( b1 == 176 && b2 == 15 ) // crossfader
    {
      int bpm = 120 + (b3 / 127.f) * 60;
      cout << "XFade " << b3 << "   setting BPM to " << bpm << endl;
      top->bpm = bpm;
    }
    
    if ( b2 == 91 ) // play
    {
      //lo_send( //lo_address_new(NULL, "14688") , "/luppp/play", "" );
    }
    if ( b2 == 92 ) // stop
    {
      //lo_send( //lo_address_new(NULL, "14688") , "/luppp/stop", "" );
    }
    if ( b2 == 93 && b1 == 144 ) // rec
    {
      std::cout << "Record pressed!" << std::endl;
      //lo_send( //lo_address_new(NULL, "14688") , "/luppp/mixer/addtrack", "" );
    }
    
    index++;
  }
}
