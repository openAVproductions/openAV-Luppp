
#include "jackclient.hpp"

#include <iostream>

#include "top.hpp"

using namespace std;

JackClient::JackClient( Top* t) :
                        mixer(t)
{
  top = t;
  
  client = jack_client_open ( "Luppp", JackNullOption , 0 , 0 );
  
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

int JackClient::process(jack_nframes_t nframes)
{
  // process incoming MIDI
  midiInputBuffer = jack_port_get_buffer (midiInputPort, nframes);
  jack_midi_event_t in_event; // event buffer
  
  int midiIndex = 0;
  int event_count = (int) jack_midi_get_event_count(midiInputBuffer); // numMessages
  
  while (midiIndex < event_count)
  {
    jack_midi_event_get(&in_event, midiInputBuffer, midiIndex); // get first message
    
    // buffer[0] == type
    // buffer[1] == message & channel
    // buffer[2] == data
    int b1 = (int)in_event.buffer[0];
    int b2 = (int)in_event.buffer[1];
    int b3 = (int)in_event.buffer[2];
    
    cout << b1 << "  " << b2 << "  " << b3 << endl;
    
    midiIndex++;
  }
  
  float* outBuffer   = (float*)jack_port_get_buffer (outputPort    , nframes);
  
  mixer.process(nframes, outBuffer);
  
  return true;
};

void JackClient::deactivate()
{
  jack_deactivate(client);
  jack_client_close (client);
  std::cout << "Jack::Deactivate()" << std::endl;
}

