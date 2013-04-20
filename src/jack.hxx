
#ifndef LUPPP_JACK_H
#define LUPPP_JACK_H

/*
    jack.hxx
  
  This code contains the JACK client.
  It allows reading / writing of audio / midi.
  
*/

// Library
#include <cstring>
#include <jack/jack.h>
#include <jack/midiport.h>

class Buffers
{
  public:
    Buffers()
    {
      memset( audio, 0, sizeof(float*)*2);
    }
    float* audio[2];
    
    enum BUFFER {
      MASTER_OUTPUT = 0,
      MASTER_INPUT,
    };
};

class Jack
{
  public:
    Jack();
    
    void activate();
    int getBuffersize();
    int getSamplerate();
  
  private:
    Buffers buffers;
    
    int nframes;
    int samplerate;
    
    // JACK member variables
    jack_client_t* client;
    
    jack_port_t*  masterInput;
    jack_port_t*  masterOutput;
    
    // JACK callbacks
    int process (jack_nframes_t);
    
    int timebase(jack_transport_state_t,
                 jack_nframes_t,
                 jack_position_t*,
                 int );
    
    // static JACK callbacks
    static int static_process  (jack_nframes_t, void *);
    
    static int static_timebase (jack_transport_state_t,
                                jack_nframes_t,
                                jack_position_t*,
                                int,
                                void* );
    
};

#endif // LUPPP_JACK_H

