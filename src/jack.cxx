
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
  
  
  loopers.push_back( new Looper() );
}


void Jack::activate()
{
  jack_activate( client );
}


int Jack::process (jack_nframes_t nframes)
{
  // do events from the ringbuffer
  handleDspEvents();
  
  // get buffers
  buffers.audio[Buffers::MASTER_INPUT]  = (float*)jack_port_get_buffer( masterInput , nframes);
  buffers.audio[Buffers::MASTER_OUTPUT] = (float*)jack_port_get_buffer( masterOutput, nframes);
  
  // pre-zero output buffers
  memset( buffers.audio[Buffers::MASTER_OUTPUT], 0, sizeof(float) * nframes );
  
  
  loopers.at(0)->process(nframes, &buffers );
  
  /*
  float* input = buffers.audio[Buffers::MASTER_INPUT];
  float* output = buffers.audio[Buffers::MASTER_OUTPUT];
  
  for(uint i = 0; i < nframes; i++)
  {
    *output++ = *input++;
  }
  */
  
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

int Jack::timebase(jack_transport_state_t state,
                   jack_nframes_t nframes,
                   jack_position_t* pos,
                   int newPos)
{
  // fill buffers with data, then pass to timeManager
  buffers.transportFrame    = jack_get_current_transport_frame(client);
  buffers.transportPosition = *pos;
  buffers.transportState    = state;
  
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
