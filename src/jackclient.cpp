
#include "jackclient.hpp"

#include <cmath>
#include <iostream>

using namespace std;

JackClient::JackClient( Top* t) :
                        mixer(t)
{
  top = t;
  
  client = jack_client_open ( "Luppp", JackNullOption , 0 , 0 );
  
  t->bufferSize = jack_get_buffer_size(client);
  
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

  mixer.addTrack();
  mixer.addTrack();

  top->state.addTrack();
  top->state.addTrack();
  
  jack_activate(client);
}

int JackClient::static_process(jack_nframes_t nframes, void* thisPointer)
{
  return static_cast<JackClient*>(thisPointer)->process(nframes);
}

int JackClient::process(jack_nframes_t nframes)
{
  float* outBuffer   = (float*)jack_port_get_buffer (outputPort    , nframes);
  
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
    
    if ( b1 >= 144 && b1 < 144 + 16 ) // note on
    {
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      std::advance(i,1);
      
      // frequency of cycles trough the buffer
      float freq = pow( 2.0, ((double)b2 - 69.0) / 12.0 );
      
      // buffer size = 512
      float cyclesTroughBuffer = freq;
      
      std::cout << "CyclesTroughBuffer = " << cyclesTroughBuffer << std::endl;
      
      i->speed = freq;
      
      std::cout << "i->speed = (freq) " << i->speed << endl;
    }
    else if ( b1 >= 128 && b1 < 128 + 16 ) // note off
    {
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      std::advance(i,1);
      i->speed = 0;
      std::cout << "i->speed = " << i->speed << endl;
    }
    else if ( b1 >= 176 && b1 < 176 + 16 ) // CC change
    {
      std::list<FileAudioSourceState>::iterator i = top->state.fileAudioSourceState.begin();
      if ( b2 == 17 )
        std::advance(i,1);
      float value = (b3/127.f);
      i->speed = value  + 0.5;
      std::cout << "i->speed = " << i->speed << endl;
      
      EngineEvent* x = new EngineEvent();
      x->setMixerVolume(0, value);
      top->toGuiQueue.push(x);
      
      top->guiDispatcher->emit();
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

