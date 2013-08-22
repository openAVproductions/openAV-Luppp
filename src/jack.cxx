
#include "jack.hxx"

#include <sstream>
#include <cstring>
#include <iostream>

#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "controller/gui.hxx"

using namespace std;

int AudioBuffer::privateID = 0;

extern int jackSamplerate;

Jack::Jack() :
  client( jack_client_open ( "Luppp", JackNullOption , 0 , 0 ) ),
  timeManager(),
  metronome( new Metronome() ),
  logic( new Logic() ),
  gridLogic( new GridLogic() ),
  controllerUpdater( new ControllerUpdater() ),
  clientActive(false)
{
  buffers.nframes = jack_get_buffer_size( client );
  buffers.samplerate = jack_get_sample_rate( client );
  
  uiUpdateCounter  = buffers.samplerate / 30;
  uiUpdateConstant = buffers.samplerate / 30;
  
  masterInput  = jack_port_register( client,
                          "master_in",
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
  
  masterMidiInput  = jack_port_register( client,
                          "midi_in",
                          JACK_DEFAULT_MIDI_TYPE,
                          JackPortIsInput,
                          0 );
  
  apcMidiInput  = jack_port_register( client,
                          "apc_in",
                          JACK_DEFAULT_MIDI_TYPE,
                          JackPortIsInput,
                          0 );
  
  apcMidiOutput  = jack_port_register( client,
                          "apc_out",
                          JACK_DEFAULT_MIDI_TYPE,
                          JackPortIsOutput,
                          0 );
  
  masterVol = 0.77;
  
  /// prepare internal buffers
  buffers.audio[Buffers::REVERB]         = new float[ buffers.nframes ];
  buffers.audio[Buffers::SIDECHAIN]      = new float[ buffers.nframes ];
  buffers.audio[Buffers::POST_SIDECHAIN] = new float[ buffers.nframes ];
  
  buffers.audio[Buffers::MASTER_OUT_L]   = new float[ buffers.nframes ];
  buffers.audio[Buffers::MASTER_OUT_R]   = new float[ buffers.nframes ];
  
  for(int i = 0; i < NTRACKS; i++)
  {
    loopers.push_back( new Looper(i) );
    trackOutputs.push_back( new TrackOutput(i, loopers.back() ) );
    
    buffers.audio[Buffers::TRACK_0 + i] = new float[ buffers.nframes ];
    
    timeManager.registerObserver( loopers.back() );
  }
  
  /// setup FX
  reverb = new Reverb( buffers.samplerate );
  reverb->dryWet( 1 );
  
  sidechainGain = new SidechainGain( buffers.samplerate );
  
  
  reverbMeter = new DBMeter( buffers.samplerate );
  masterMeter = new DBMeter( buffers.samplerate );
  
  /// setup JACK callbacks
  if ( jack_set_process_callback( client,
                                  static_process,
                                  static_cast<void*>(this)) )
  {
    cerr << "Jack() error setting process callback" << endl;
  }
  
  if ( jack_set_timebase_callback(client,
                                  0,
                                  (JackTimebaseCallback)static_timebase,
                                  static_cast<void*>(this)) )
  {
    cerr << "Jack() error setting timebase callback" << endl;
  }
}


void Jack::activate()
{
  // move to "settings" class or so
  Controller* c = new AkaiAPC();
  controllerUpdater->registerController( c );
  Controller* g = new LupppGUI();
  controllerUpdater->registerController( g );
  
  // move to time class, get instantiate order right
  jack->getTimeManager()->registerObserver( metronome );
  jack->getTimeManager()->registerObserver( gridLogic );
  
  jack_activate( client );
  jack_transport_start(client);
}


int Jack::process (jack_nframes_t nframes)
{
  
  /// get buffers
  buffers.audio[Buffers::MASTER_INPUT]        = (float*)jack_port_get_buffer( masterInput    , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_L]   = (float*)jack_port_get_buffer( masterOutputL  , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_R]   = (float*)jack_port_get_buffer( masterOutputR  , nframes );
  buffers.midi [Buffers::MASTER_MIDI_INPUT]   = (void*) jack_port_get_buffer( masterMidiInput, nframes );
  buffers.midi [Buffers::APC_INPUT]           = (void*) jack_port_get_buffer( apcMidiInput   , nframes );
  buffers.midi [Buffers::APC_OUTPUT]          = (void*) jack_port_get_buffer( apcMidiOutput  , nframes );
  
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_L] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_R] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_L]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_R]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::REVERB]            , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN]         , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::POST_SIDECHAIN]    , 0, sizeof(float) * nframes );
  
  jack_midi_clear_buffer( buffers.midi[Buffers::APC_OUTPUT] );
  
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
    
    // run each event trought the midiObservers vector
    for(unsigned int i = 0; i < midiObservers.size(); i++ )
    {
      midiObservers.at(i)->midi( (unsigned char*) &in_event.buffer[0] );
    }
    
    masterMidiInputIndex++;
  }
  
  /// process each track, starting at output and working up signal path
  for(unsigned int i = 0; i < NTRACKS; i++)
  {
    trackOutputs.at(i)->process( nframes, &buffers );
  }
  
  /// process reverb
  float* buf[] = {
    buffers.audio[Buffers::REVERB],
    buffers.audio[Buffers::REVERB]
  };
  //if ( reverb->getActive() )
  {
    reverbMeter->process(nframes, buffers.audio[Buffers::REVERB], buffers.audio[Buffers::REVERB] );
    reverb->process( nframes, &buf[0], &buf[0] );
  }
  
  
  /// process sidechain gaining
  float* sidechainBuf[] = {
    // input
    buffers.audio[Buffers::MASTER_OUT_L],
    buffers.audio[Buffers::MASTER_OUT_R],
    buffers.audio[Buffers::SIDECHAIN],
    // output
    buffers.audio[Buffers::MASTER_OUT_L],
    buffers.audio[Buffers::MASTER_OUT_R]
  };
  
  sidechainGain->process( nframes, &sidechainBuf[0], &sidechainBuf[3] );
  
  /// metro signal after sidechain-gain, so its loud on the beats
  metronome->process( nframes, &buffers );
  
  /// mix reverb & post-sidechain in
  for(unsigned int i = 0; i < buffers.nframes; i++)
  {
    float L    = buffers.audio[Buffers::MASTER_OUT_L][i];
    float R    = buffers.audio[Buffers::MASTER_OUT_R][i];
    float rev  = buffers.audio[Buffers::REVERB][i];
    float post = buffers.audio[Buffers::POST_SIDECHAIN][i];
    
    buffers.audio[Buffers::MASTER_OUT_L][i] = (L + rev + post) * masterVol;
    buffers.audio[Buffers::MASTER_OUT_R][i] = (R + rev + post) * masterVol;
  }
  
  
  /// db meter on master
  masterMeter->process(nframes, buffers.audio[Buffers::MASTER_OUT_L], buffers.audio[Buffers::MASTER_OUT_R] );
  
  if ( uiUpdateCounter > uiUpdateConstant )
  {
    EventTrackSignalLevel e(-1, masterMeter->getLeftDB(), masterMeter->getRightDB() );
    writeToGuiRingbuffer( &e );
    
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  
  // memcpy the internal MASTER_OUTPUT buffer to the JACK_MASTER_OUTPUT
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_L],
          buffers.audio[Buffers::MASTER_OUT_L],
          //buffers.audio[Buffers::TRACK_7],
          sizeof(float)*nframes);
  
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_R],
          buffers.audio[Buffers::MASTER_OUT_R],
          //buffers.audio[Buffers::POST_SIDECHAIN],
          //buffers.audio[Buffers::REVERB],  // uncomment to listen to reverb send only
          sizeof(float)*nframes);
  
  return false;
}

void Jack::setReverb( bool a, float d, float s )
{
  //reverb->setActive( a );
  reverb->damping( d );
  reverb->rt60( d );
}

int Jack::getBuffersize()
{
  return jack_get_buffer_size( client );
}

int Jack::getSamplerate()
{
  return jack_get_sample_rate( client );
}

void Jack::writeApcOutput( unsigned char* data )
{
  void* apcOutput   = buffers.midi[Buffers::APC_OUTPUT];
  
  unsigned char* buf = jack_midi_event_reserve( apcOutput, 0, 3);
  if( buf != 0 )
  {
    memcpy( buf, data, sizeof( unsigned char ) * 3);
    //cout << "writeApcOutput " << int(buf[0]) << ", " << int(buf[1]) << ", " << int(buf[2]) << endl;
  }
  else
  {
    EventGuiPrint e( "__FILE__ __LINE__ Buffer not valid" );
    writeToGuiRingbuffer( &e );
  }
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
  timeManager.process( &buffers );
  
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
