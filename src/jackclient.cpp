
#include "jackclient.hpp"

#include <cmath>
#include <iostream>

#include "effectstate.hpp"
#include "audiobuffer.hpp"

using namespace std;

JackClient::JackClient( Top* t) :
                        mixer(t)
{
  top = t;
  
  client = jack_client_open ( "Luppp", JackNullOption , 0 , 0 );
  
  t->bufferSize = jack_get_buffer_size(client);
  t->samplerate = jack_get_sample_rate(client);
  
  inputPort = jack_port_register ( client,
                                    "input",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput,
                                    0 );
  
  outputPort = jack_port_register ( client,
                                    "output",
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

  mixer.addTrack();
  mixer.addTrack();
  //mixer.addTrack();

  top->state.addTrack();
  top->state.addTrack();
  //top->state.addTrack();
  
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
      cout << "STATE_NEW_EFFECT, ID = " << flush;
      Effect* effect = (Effect*)e->vPtr;
      cout << effect->getID() << endl;
      mixer.addEffect(e->ia, e->ib, effect);
    }
    else if ( e->type == EE_LOOPER_LOAD ) {
      // insert the correct buffer ID into the list, so we can retrieve by sceneID later
      cout << "EE_LOOPER_LOAD " << e->ia << ", " << e->ib << "," << e->ic << endl;
      top->state.setClipSelectorState( e->ia, e->ib, e->ic );
    }
    else if ( e->type == EE_LOOPER_SELECT_BUFFER) {
      // tells the AudioSource @ track ia to play scene ib
      cout << "EE_LOOPER_SELECT_BUFFER " << e->ia << ", " << e->ib << endl;
      top->state.clipSelectorQueue(e->ia, e->ib);
    }
    else if ( e->type == EE_TRACK_SET_PLUGIN_PARAMETER) {
      //cout << "EE_TRACK_SET_PLUGIN_PARAMETER " << e->ia << ", " << e->ib<< ", " << e->ic << ", " << e->fa << endl;
      
      if ( e->ic == 1 )
      {
        //top->state.numPoles = e->fa;
      }
      else
      {
        top->state.cutoff = e->fa;
      }
    }
    
  }
  
  return 0;
}

int JackClient::process(jack_nframes_t nframes)
{
  float* outBuffer   = (float*)jack_port_get_buffer (outputPort    , nframes);
  
  processRtQueue();
  
  apcRead(nframes);
  
  // handle incoming midi
  processMidi(nframes);
  
  mixer.process(nframes, outBuffer);
  
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
      
      EngineEvent* x = new EngineEvent();
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
    buffer[0] = b1;
    buffer[1] = b2;
    buffer[2] = b3;
  }
}

void JackClient::apcRead( int nframes )
{
  void* apcInBuffer = jack_port_get_buffer ( apcInputPort, nframes);
  void* apcOutBuffer = jack_port_get_buffer ( apcOutputPort, nframes);
  
  jack_midi_clear_buffer(apcOutBuffer);
  
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
    
    /*
    // apc 40 Device Control On / Off
    if( b2 == 59 )
    {
      if ( b1 >= 144 && b1 < 144 + 16 )
      {
        if ( b1 - 144 < trackStateVector.size() )
        {
          std::cout << "Track ID " << b1 - 144 << " Active = true" << std::flush;
          int selDev = trackStateVector.at(b1 - 144)->selectedDevice;
          std::cout << "  SelDev = " << selDev << std::endl;
          trackStateVector.at(b1 - 144)->deviceState.at(selDev)->active = true;
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/device/active", "iii", b1 - 144, selDev, 1 );
        }
        else
        {
          std::cout << "Track ID " << b1 - 144 << " out of range!" << std::endl;
        }
      }
      else if ( b1 >= 128 && b1 < 128 + 16 ) // off
      {
        if ( b1 - 128 < trackStateVector.size() )
        {
          std::cout << "Track ID " << b1 - 128 << " Active = false" << std::endl;
          int selDev = trackStateVector.at(b1 - 128)->selectedDevice;
          trackStateVector.at(b1 - 128)->deviceState.at(trackStateVector.at(b1 - 128)->selectedDevice)->active = true;
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/device/active", "iii", b1 - 128, selDev, 0 );
        }
        else
        {
          std::cout << "Track ID " << b1 - 128 << " out of range!" << std::endl;
        }
      }
    }
    */
    
    // APc <- Selected device button
    if( b2 == 60 && ( b1 >= 144 && b1 < 144 + 16 ) )
    {
      int trackID = b1 - 144;
      int device = top->controller->getDevice();
      if ( device > 0 ) // range check
        top->controller->setTrackDevice(trackID, --device);
      std::cout << "APC <- Device button: NOTE ON track " << trackID << " device " << device << std::endl;
      writeMidi( apcOutBuffer, 128 + trackID, 60, 127 );
    }
    
    // Apc -> selected device button
    if( b2 == 61 && ( b1 >= 144 && b1 < 144 + 16 ) )
    {
      int trackID = b1 - 144;
      int device = top->controller->getDevice();
      if ( device < 9 )
        top->controller->setTrackDevice(trackID,++device);
      std::cout << "APC -> Device button: NOTE ON track = " << trackID << " device " << device << std::endl;
      writeMidi( apcOutBuffer, 128 + trackID, 61, 127 );
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
      writeMidi( apcOutBuffer, 128, trackControlMode, 0); // turn off previous mode
      trackControlMode = (ApcTrackControlMode)b2;
      
      // turn on new mode
      writeMidi( apcOutBuffer, 144, trackControlMode, 127);
      
      
    }
    
    // apc 40 top controllers (bank of 8)
    if ( b1 == 176 && b2 >= 48 && b2 < 56 )
    {
      int trackID;
      float value;
      switch ( trackControlMode )
      {
        case APC_TRACK_CONTROL_MODE_PAN:
          trackID = b2 - 48;
          value = (b3 / 63.f) - 1.f;
          std::cout << "Track Control: PAN MODE:  trackID: " << trackID << "  value " << value << std::endl;
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/mixer/pan", "if", trackID, value );
          break;
        case APC_TRACK_CONTROL_MODE_SEND_A:
          trackID = b2 - 48;
          value = (b3 / 63.f) - 1.f;
          std::cout << "Track Control: SEND A:  trackID: " << -2 << "  value " << value << std::endl;
          //lo_send( //lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameter", "iiif", -2, 1, b2 - 47, b3 / 127.f );
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
        top->state.clipSelectorQueue(id, val);
        
        
        writeMidi( apcOutBuffer, b1, b2, 127);
      }
    }
    
    // APC 40 grid RELEASE events
    if ( b1 >= 128 && b1 < 128 + 16 )
    {
      if ( b2 >= 52 && b2 < 58 ) // the 5 vertial "launch" buttons, AND the clip stop (#52)
      {
        /*
        int id = b1 - 128;
        int val= b2 - 53; // here we take 53 away, so the clip stop button will result in -1 for buffer ID
        
        
        // if based on mode
        if ( apcMode == APC_MODE_CLIP )
        {
          std::cout << "SETTING BUFFER FROM APC40! ID:" << id << " val: " << val << std::endl;
          //lo_send( //lo_address_new(NULL, "14688") , "/luppp/looper/selectbuffer", "ii", id, val );
        }
        else
        {
          int sampleNum = -1;
          std::cout << "SAMPLER STOP B2 == " << b2 << "  ID = " << id << std::endl;
          //lo_send( //lo_address_new(NULL, "14688") , "/luppp/sampler/stop", "i", ( -((b2 - 57))*4) + id);
        }
        */
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
        
        EffectState* state = top->state.getEffectState(track);
        if ( state )
        {
          cout << "State->values 0  " << state->values[0] << endl;
        }
        
        top->state.cutoff = b3/127.;
        EngineEvent* x = new EngineEvent();
        x->setPluginParameter(track,
                              device,
                              b2-16,
                              b3 / 127.);
        top->toGuiQueue.push(x);
        
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
        //lo_send( //lo_address_new(NULL, "14688") , "/luppp/track/record", "ii", trackID, 1 );
      }
      else if ( b1 >= 128 && b1 < 128 + 16 ) // rec off
      {
        int trackID = b1 - 128;
        std::cout << "APC: REC on track " << trackID << " off!" << std::endl;
        //lo_send( //lo_address_new(NULL, "14688") , "/luppp/track/record", "ii", trackID, 0 );
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
      if ( b1 >= 144 && b1 < 144 + 16 ) // mute off, ie ACTIVE track (led ON)
      {
        int trackID = b1 - 144;
        std::cout << "APC: Mute on track " << trackID << " off!" << std::endl;
        top->state.setMute( trackID, false );
        EngineEvent* x = new EngineEvent();
        x->setTrackMute(trackID, false);
        top->toGuiQueue.push(x);
        top->guiDispatcher->emit();
      }
      else if ( b1 >= 128 && b1 < 128 + 16 ) // for note off
      {
        int trackID = b1 - 128;
        std::cout << "APC: Mute on track " << trackID << " on!" << std::endl;
        top->state.setMute( trackID, true );
        EngineEvent* x = new EngineEvent();
        x->setTrackMute(trackID, true);
        top->toGuiQueue.push(x);
        top->guiDispatcher->emit();
      }
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
