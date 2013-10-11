
#include "midi.hxx"

#include <sstream>

#include "../jack.hxx"

extern Jack* jack;

MidiIO::MidiIO() :
  jackInputPort(0),
  jackOutputPort(0)
{
  LUPPP_NOTE("MidiIO %i",this);
}


void MidiIO::writeMidi( unsigned char* data )
{
  void* portBuffer = jack_port_get_buffer( jackOutputPort, jack->getBuffersize() );
  
  unsigned char* buffer = jack_midi_event_reserve( portBuffer, 0, 3);
  if( buffer == 0 )
  {
    std::cout << "Error: APC writeMidi() write buffer == 0" << std::endl;
    return;
  }
  else
  {
    cout << "JC::writeMidi() port =  " << jackOutputPort << int(data[0]) << ", " << int(data[1]) << ", " << int(data[2]) << endl; 
    //memcpy( buffer, data, sizeof(unsigned char)*3 );
    buffer[0] = data[0];
    buffer[1] = data[1];
    buffer[2] = data[2];
  }
  
}

int MidiIO::registerMidiPorts(std::string name)
{
  // register the JACK MIDI ports
  stringstream i;
  i << name << " in";
  jackInputPort  = jack_port_register( jack->getJackClientPointer(),
                                              i.str().c_str(),
                                              JACK_DEFAULT_MIDI_TYPE,
                                              JackPortIsInput,
                                              0 );
  stringstream o;
  o << name << " out";
  jackOutputPort  = jack_port_register( jack->getJackClientPointer(),
                                              o.str().c_str(),
                                              JACK_DEFAULT_MIDI_TYPE,
                                              JackPortIsOutput,
                                              0 );
  
  if ( jackInputPort && jackOutputPort )
  {
    //cout << jackOutputPort << endl;
    //LUPPP_NOTE("%i, %i", jackInputPort, jackOutputPort );
    return LUPPP_RETURN_OK;
  }
  else
  {
    LUPPP_ERROR("Error registering JACK ports" );
    return LUPPP_RETURN_ERROR;
  }
}


void MidiIO::initBuffers(int nframes)
{
  // clear the output buffer
  void* outputBuffer= (void*) jack_port_get_buffer( jackOutputPort, nframes );
  jack_midi_clear_buffer( outputBuffer );
}

void MidiIO::process(int nframes)
{
  // get port buffers and setup
  void* inputBuffer = (void*) jack_port_get_buffer( jackInputPort, nframes );
  
  jack_midi_event_t in_event;
  int index = 0;
  int event_count = (int) jack_midi_get_event_count( inputBuffer );
  while ( index < event_count )
  {
    jack_midi_event_get(&in_event, inputBuffer, index);
    midi( (unsigned char*) &in_event.buffer[0] );
    printf( "MIDI %i %i %i\n", int(in_event.buffer[0]), int(in_event.buffer[1]), int(in_event.buffer[2]) );
    index++;
  }
  
}
