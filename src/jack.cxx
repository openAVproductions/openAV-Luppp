
#include "jack.hxx"

#include <sstream>
#include <cstring>
#include <iostream>

#include "eventhandler.hxx"

using namespace std;

extern int jackSamplerate;

Jack::Jack()
{
  // open the client
  client = jack_client_open ( "Luppp", JackNullOption , 0 , 0 );
  
  buffers.nframes = jack_get_buffer_size( client );
  buffers.samplerate = jack_get_sample_rate( client );
  
  uiUpdateCounter  = buffers.samplerate / 30;
  uiUpdateConstant = buffers.samplerate / 30;
  
  masterOutput = jack_port_register( client,
                          "master_out",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  masterInput  = jack_port_register( client,
                          "master_in",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
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
  
  
  /// prepare internal buffers
  buffers.audio[Buffers::REVERB]         = (float*) malloc( sizeof(float) * nframes );
  buffers.audio[Buffers::SIDECHAIN]      = (float*) malloc( sizeof(float) * nframes );
  buffers.audio[Buffers::POST_SIDECHAIN] = (float*) malloc( sizeof(float) * nframes );
  buffers.audio[Buffers::MASTER_OUTPUT]  = (float*) malloc( sizeof(float) * nframes );
  
  for(int i = 0; i < NTRACKS; i++)
  {
    loopers.push_back( new Looper(i) );
    timeManager.registerObserver( loopers.at(i) );
    
    buffers.audio[Buffers::TRACK_0 + i] = (float*) malloc( sizeof(float) * nframes );
  }
  
  for( int i = 0; i < NTRACKS; i++)
  {
    trackOutputs.push_back( new TrackOutput(i, loopers.at(i) ) );
  }
  
  timeManager.registerObserver( &metronome );
  
  /// setup FX
  reverb = new Reverb( buffers.samplerate );
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
  jack_activate( client );
  jack_transport_start(client);
}


int Jack::process (jack_nframes_t nframes)
{
  // get buffers
  buffers.audio[Buffers::MASTER_INPUT]    = (float*)jack_port_get_buffer( masterInput , nframes);
  buffers.audio[Buffers::JACK_MASTER_OUTPUT]=(float*)jack_port_get_buffer(masterOutput, nframes);
  buffers.midi[Buffers::MASTER_MIDI_INPUT]= (void*) jack_port_get_buffer( masterMidiInput, nframes );
  buffers.midi[Buffers::APC_INPUT]        = (void*) jack_port_get_buffer( apcMidiInput   , nframes );
  buffers.midi[Buffers::APC_OUTPUT]       = (void*) jack_port_get_buffer( apcMidiOutput  , nframes );
  
  // pre-zero output buffers
  memset( buffers.audio[Buffers::MASTER_OUTPUT], 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::JACK_MASTER_OUTPUT],0,sizeof(float)*nframes);
  memset( buffers.audio[Buffers::REVERB]       , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN]    , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::POST_SIDECHAIN],0, sizeof(float) * nframes );
  
  for(int i = 0; i < NTRACKS; i++)
    memset( buffers.audio[Buffers::TRACK_0 + i], 0, sizeof(float) * nframes );
  
  jack_midi_clear_buffer( buffers.midi[Buffers::APC_OUTPUT] );
  
  
  // do events from the ringbuffer
  handleDspEvents();
  
  // process incoming MIDI
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
    
    // check each looper for MIDI match
    for(int i = 0; i < loopers.size(); i++)
      loopers.at(i)->midi( (unsigned char*)&in_event.buffer[0] );
    
    masterMidiInputIndex++;
  }
  
  // process each track, starting at output and working up signal path
  for(uint i = 0; i < NTRACKS; i++)
    loopers.at(i)->process( nframes, &buffers );
  
  // process fx
  float* buf[] = {
    buffers.audio[Buffers::REVERB],
    buffers.audio[Buffers::REVERB],
    buffers.audio[Buffers::REVERB],
    buffers.audio[Buffers::REVERB],
  };
  
  //reverbMeter->process(nframes, buffers.audio[Buffers::REVERB], buffers.audio[Buffers::REVERB] );
  //reverb->process( nframes, &buf[0], &buf[2] );
  
  // db meter on master output, then memcpy to JACK
  masterMeter->process(nframes, buffers.audio[Buffers::MASTER_OUTPUT], buffers.audio[Buffers::MASTER_OUTPUT] );
  
  metronome.process( nframes, &buffers );
  
  if ( uiUpdateCounter > uiUpdateConstant )
  {
    float peak = masterMeter->getLeftDB();
    EventTrackSignalLevel e(-1, peak, masterMeter->getRightDB() );
    writeToGuiRingbuffer( &e );
    
    char buf[50];
    snprintf( buf, 50, "signal: %f", peak );
    EventGuiPrint e2( buf );
    writeToGuiRingbuffer(&e2);
    
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  // memcpy the internal MASTER_OUTPUT buffer to the JACK_MASTER_OUTPUT
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUTPUT],
          buffers.audio[Buffers::MASTER_OUTPUT],
          sizeof(float)*nframes);
  
  
  return false;
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
