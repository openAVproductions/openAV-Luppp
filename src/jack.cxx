
#include "jack.hxx"

#include <sstream>
#include <cstring>
#include <iostream>

#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "controller/guicontroller.hxx"
#include "controller/genericmidi.hxx"
#include "controller/apc.hxx"

using namespace std;

int AudioBuffer::privateID = 0;

// static pointer from main.
extern Jack* jack;

extern int jackSamplerate;

Jack::Jack() :
  client( jack_client_open ( "Luppp", JackNullOption , 0 , 0 ) ),
  state( new State() ),
  controllerUpdater( new ControllerUpdater() ),
  clientActive(false)
{
  jack = this;
  // construct Observer classes here, not in the initializer list as the Jack*
  // will be 0x0 until then.
  timeManager = new TimeManager(),
  metronome = new Metronome();
  logic = new Logic();
  gridLogic = new GridLogic();
  
  
  // CAREFUL: setup the size by default: otherwise malloc() is called on push_back
  midiIO.reserve( CONTROLLERS_PREALLOC );
  
  //GenericMIDI* tmp = new GenericMIDI("akai_apc.ctlr");
  //tmp->registerComponents();
  //controllerUpdater->registerController( static_cast<Controller*>(tmp) );
  
  buffers.nframes = jack_get_buffer_size( client );
  buffers.samplerate = jack_get_sample_rate( client );
  
  EventSamplerate e(buffers.samplerate);
  writeToGuiRingbuffer( &e );
  
  uiUpdateCounter  = buffers.samplerate / 30;
  uiUpdateConstant = buffers.samplerate / 30;
  
  masterInput  = jack_port_register( client,
                          "master_in",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterReturnL = jack_port_register( client,
                          "master_return_left",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterReturnR = jack_port_register( client,
                          "master_return_right",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterOutputL = jack_port_register( client,
                          "master_left",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  masterOutputR = jack_port_register( client,
                          "master_right",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sendOutput    = jack_port_register( client,
                          "send_out",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sidechainKeyOutput= jack_port_register( client,
                          "sidechain_key",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sidechainSignalOutput= jack_port_register( client,
                          "sidechain_signal",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  masterMidiInput  = jack_port_register( client,
                          "midi_in",
                          JACK_DEFAULT_MIDI_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterVol = 0.0f;
  returnVol = 1.0f;
  
  /// prepare internal buffers
  buffers.audio[Buffers::SEND]           = new float[ buffers.nframes ];
  buffers.audio[Buffers::SIDECHAIN_KEY]  = new float[ buffers.nframes ];
  buffers.audio[Buffers::SIDECHAIN_SIGNAL]=new float[ buffers.nframes ];
  
  buffers.audio[Buffers::MASTER_OUT_L]   = new float[ buffers.nframes ];
  buffers.audio[Buffers::MASTER_OUT_R]   = new float[ buffers.nframes ];
  
  for(int i = 0; i < NTRACKS; i++)
  {
    loopers.push_back( new Looper(i) );
    trackOutputs.push_back( new TrackOutput(i, loopers.back() ) );
    
    buffers.audio[Buffers::TRACK_0 + i] = new float[ buffers.nframes ];
    
    timeManager->registerObserver( loopers.back() );
  }
  
  /// setup DSP instances
  inputVol = 1.0f;
  masterVol = 0.75f;
  masterMeter = new DBMeter( buffers.samplerate );
  inputMeter  = new DBMeter( buffers.samplerate );
  
  /// setup JACK callbacks
  if ( jack_set_process_callback( client,
                                  static_process,
                                  static_cast<void*>(this)) )
  {
    LUPPP_ERROR("%s","Error setting process callback");
  }
  
  if ( jack_set_timebase_callback(client,
                                  0,
                                  (JackTimebaseCallback)static_timebase,
                                  static_cast<void*>(this)) )
  {
    LUPPP_ERROR("%s","Error setting timebase callback");
  }
}

Jack::~Jack()
{
  delete timeManager;
  delete metronome;
  delete state;
  delete logic;
  delete gridLogic;
  delete controllerUpdater;
  
  delete inputMeter;
  delete masterMeter;
}

void Jack::activate()
{
  //Controller* m = new AkaiAPC();
  
  // TODO: Add GUI dialog to add controllers, and insert them into the controller map.
  // Watch out for RT stuff, loading file, registering ports etc: before activate?!
  //Controller* m = new GenericMIDI("akai_apc.ctlr","apc");
  
  Controller* gui = new LupppGUI();
  controllerUpdater->registerController( gui );
  
  if ( !gui )
  {
    LUPPP_ERROR("%s","Error creating LupppGUI Controller instance");
  }
  
  jack_activate( client );
  jack_transport_start(client);
}

void Jack::quit()
{
  jack_deactivate( client );
  jack_client_close( client );
  LUPPP_NOTE("%s","Quit JACK.");
}

TrackOutput* Jack::getTrackOutput(int t)
{
  if ( t >= 0 && t < NTRACKS )
    return trackOutputs.at(t);
#ifdef DEBUG_TRACKS
  else
  {
    printf( "Jack::getTrackOutput() returning 0x0: invalid track requested!\n" );
  }
#endif
  
  return 0;
}


Looper* Jack::getLooper(int t)
{
  if ( t >= 0 && t < NTRACKS )
    return loopers.at(t);
#ifdef DEBUG_TRACKS
  else
  {
    printf( "Jack::getLooper() returning 0x0: invalid track requested!\n" );
  }
#endif
  
  return 0;
}


void Jack::registerMidiIO( MidiIO* mo )
{
  //LUPPP_NOTE("Jack::registerMidiIO()" );
  
  // CAREFUL : this could need to resize and cause malloc() in RT thread
  midiIO.push_back( mo );
}

void Jack::unregisterMidiIO( MidiIO* mo )
{
  LUPPP_NOTE("Jack::unregisterMidiIO()");
  
  // unregister the observer
  //midiIO.push_back( mo );
}


int Jack::process (jack_nframes_t nframes)
{
  
  /// get buffers
  buffers.audio[Buffers::MASTER_INPUT]        = (float*)jack_port_get_buffer( masterInput    , nframes );
  buffers.audio[Buffers::MASTER_RETURN_L]     = (float*)jack_port_get_buffer( masterReturnL  , nframes );
  buffers.audio[Buffers::MASTER_RETURN_R]     = (float*)jack_port_get_buffer( masterReturnR  , nframes );
  
  buffers.audio[Buffers::JACK_SEND_OUT]       = (float*)jack_port_get_buffer( sendOutput     , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_L]   = (float*)jack_port_get_buffer( masterOutputL  , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_R]   = (float*)jack_port_get_buffer( masterOutputR  , nframes );
  buffers.audio[Buffers::JACK_SIDECHAIN_KEY]   = (float*)jack_port_get_buffer(sidechainKeyOutput,nframes);
  buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL]=(float*)jack_port_get_buffer(sidechainSignalOutput,nframes);
  
  buffers.midi [Buffers::MASTER_MIDI_INPUT]   = (void*) jack_port_get_buffer( masterMidiInput, nframes );
  
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_L] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_R] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_L]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_R]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SEND]              , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN_KEY]     , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN_SIGNAL]  , 0, sizeof(float) * nframes );
  
  
  /// init buffers for each MidiIO
  for(unsigned int i = 0; i < midiIO.size(); i++ )
  {
    midiIO.at(i)->initBuffers( nframes );
  }
  
  /// do events from the ringbuffer
  handleDspEvents();
  
  /// process incoming MIDI
  jack_midi_event_t in_event;
  int masterMidiInputIndex = 0;
  int event_count = (int) jack_midi_get_event_count( buffers.midi[Buffers::MASTER_MIDI_INPUT] );
  while ( masterMidiInputIndex < event_count )
  {
    jack_midi_event_get(&in_event, buffers.midi[Buffers::MASTER_MIDI_INPUT], masterMidiInputIndex);
    
    char buffer [50];
    sprintf (buffer, "MIDI %i %i %i", int(in_event.buffer[0]), int(in_event.buffer[1]), int(in_event.buffer[2]) );
    EventGuiPrint e( buffer );
    writeToGuiRingbuffer( &e );
    
    masterMidiInputIndex++;
  }
  
  /// process each MidiIO registered MIDI port
  for(unsigned int i = 0; i < midiIO.size(); i++ )
  {
    midiIO.at(i)->process( nframes );
  }
  
  /// process each track, starting at output and working up signal path
  for(unsigned int i = 0; i < NTRACKS; i++)
  {
    trackOutputs.at(i)->process( nframes, &buffers );
  }
  
  /// metro signal
  metronome->process( nframes, &buffers );
  
  /// mix input, reverb & post-sidechain in
  for(unsigned int i = 0; i < buffers.nframes; i++)
  {
    float input= buffers.audio[Buffers::MASTER_INPUT][i] * inputVol;
    
    float L    = buffers.audio[Buffers::MASTER_OUT_L][i];
    float R    = buffers.audio[Buffers::MASTER_OUT_R][i];
    float returnL = buffers.audio[Buffers::MASTER_RETURN_L][i];
    float returnR = buffers.audio[Buffers::MASTER_RETURN_R][i];
    
    if ( inputToMixEnable )
    {
      // if sending to mix, scale by volume *and* by XSide send
      float tmp = input * inputToMixVol * (1-inputToXSideVol);
      L += tmp;
      R += tmp;
    }
    if ( inputToSendEnable )
    {
      // post-mix-send amount: hence * inputToMixVol
      buffers.audio[Buffers::SEND][i] += input * inputToSendVol * inputToMixVol;
    }
    if ( inputToKeyEnable )
    {
      buffers.audio[Buffers::SIDECHAIN_KEY][i] += input;
    }
    
    buffers.audio[Buffers::SIDECHAIN_SIGNAL][i] += input * inputToXSideVol;
    
    /// mixdown returns into master buffers
    buffers.audio[Buffers::MASTER_OUT_L][i] = (L + returnL) * masterVol;
    buffers.audio[Buffers::MASTER_OUT_R][i] = (R + returnR) * masterVol;
    
    /// write SEND content to JACK port
    buffers.audio[Buffers::JACK_SEND_OUT][i] = buffers.audio[Buffers::SEND][i];
    buffers.audio[Buffers::JACK_SIDECHAIN_KEY][i] = buffers.audio[Buffers::SIDECHAIN_KEY][i];
    buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL][i] = buffers.audio[Buffers::SIDECHAIN_SIGNAL][i];
  }
  
  
  /// db meter on master input & output
  inputMeter->process( nframes, buffers.audio[Buffers::MASTER_INPUT], buffers.audio[Buffers::MASTER_INPUT]);
  masterMeter->process(nframes, buffers.audio[Buffers::MASTER_OUT_L], buffers.audio[Buffers::MASTER_OUT_R] );
  
  if ( uiUpdateCounter > uiUpdateConstant )
  {
    // instead of scaling whole buffer, just scale output by vol
    EventTrackSignalLevel e(-1, masterMeter->getLeftDB(), masterMeter->getRightDB() );
    writeToGuiRingbuffer( &e );
    EventTrackSignalLevel e2(-2, inputMeter->getLeftDB() * inputVol, inputMeter->getRightDB() * inputVol );
    writeToGuiRingbuffer( &e2 );
    
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  
  // memcpy the internal MASTER_OUTPUT buffer to the JACK_MASTER_OUTPUT
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_L],
          buffers.audio[Buffers::MASTER_OUT_L],
          sizeof(float)*nframes);
  
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_R],
          buffers.audio[Buffers::MASTER_OUT_R],
          //buffers.audio[Buffers::POST_SIDECHAIN],
          //buffers.audio[Buffers::SEND],  // uncomment to listen to reverb send only
          sizeof(float)*nframes);
  
  return false;
}

void Jack::masterVolume(float vol)
{
  masterVol = vol;
}

void Jack::returnVolume(float vol)
{
  returnVol = vol;
}

void Jack::inputVolume(float v)
{
  inputVol = v * 2;
}

void Jack::inputTo(INPUT_TO to, float v)
{
  switch ( to )
  {
    case INPUT_TO_MIX:
        inputToMixVol = v;
        break;
    case INPUT_TO_SEND:
        inputToSendVol = v;
        break;
    case INPUT_TO_XSIDE:
        inputToXSideVol = v;
        break;
    default:
        break;
  }
}

void Jack::inputToActive(INPUT_TO to, bool a)
{
  switch ( to )
  {
    case INPUT_TO_MIX:
        inputToMixEnable = a;
        break;
    case INPUT_TO_SEND:
        inputToSendEnable = a;
        break;
    case INPUT_TO_SIDE_KEY:
        inputToKeyEnable = a;
        break;
    default:
        break;
  }
}

int Jack::getBuffersize()
{
  return jack_get_buffer_size( client );
}

int Jack::getSamplerate()
{
  return jack_get_sample_rate( client );
}

int Jack::timebase(jack_transport_state_t state,
                   jack_nframes_t nframes,
                   jack_position_t* pos,
                   int newPos)
{
  // fill buffers with data, then pass to timeManager
  buffers.transportFrame    = jack_get_current_transport_frame(client);
  buffers.transportPosition = pos;
  buffers.transportState    =&state;
  
  // update "time" from JACK master, or write master?
  timeManager->process( &buffers );
  
  return 0;
}

int Jack::static_process(jack_nframes_t nframes, void *instance)
{
  return static_cast<Jack*>(instance)->process(nframes);
}

int Jack::static_timebase(jack_transport_state_t state,
                          jack_nframes_t nframes,
                          jack_position_t* pos,
                          int newPos,
                          void* instance)
{
  return static_cast<Jack*>(instance)->timebase(state,nframes, pos, newPos );
}
