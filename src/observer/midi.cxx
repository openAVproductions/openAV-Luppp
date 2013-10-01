
#include "midi.hxx"

#include <sstream>

#include "../jack.hxx"

extern Jack* jack;

MidiObserver::MidiObserver()
{
  
}


void MidiObserver::registerMidiPorts(std::string name)
{
  // register the JACK MIDI ports
  stringstream i;
  i << name << "_in";
  jackInputPort  = (void*)jack_port_register( jack->getJackClientPointer(),
                                              i.str().c_str(),
                                              JACK_DEFAULT_MIDI_TYPE,
                                              JackPortIsInput,
                                              0 );
  stringstream o;
  o << name << "_out";
  jackOutputPort  = (void*)jack_port_register( jack->getJackClientPointer(),
                                              o.str().c_str(),
                                              JACK_DEFAULT_MIDI_TYPE,
                                              JackPortIsOutput,
                                              0 );
  
  LUPPP_NOTE("%i, %i", jackInputPort, jackOutputPort );
}

void MidiObserver::process(int nframes)
{
  // get port buffers and setup
  void* inputBuffer = (void*) jack_port_get_buffer( (jack_port_t*)jackInputPort, nframes );
  void* outputBuffer= (void*) jack_port_get_buffer( (jack_port_t*)jackInputPort, nframes );
  jack_midi_clear_buffer( outputBuffer );
  
  jack_midi_event_t in_event;
  int index = 0;
  int event_count = (int) jack_midi_get_event_count( inputBuffer );
  while ( index < event_count )
  {
    jack_midi_event_get(&in_event, inputBuffer, index);
    midi( (unsigned char*) &in_event.buffer[0] );
    //printf( "%s MIDI %i %i %i\n", midiObservers.at(i)->getName().c_str(), int(in_event.buffer[0]), int(in_event.buffer[1]), int(in_event.buffer[2]) );
    index++;
  }
  
}
