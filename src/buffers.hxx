
#ifndef LUPPP_BUFFERS_H
#define LUPPP_BUFFERS_H

#include <cstring>
#include <jack/transport.h>

class Buffers
{
  public:
    Buffers()
    {
      memset( audio, 0, sizeof(float*)*32);
      memset( midi , 0, sizeof(void *)*32);
    }
    float* audio[32];
    void*  midi [32];
    
    enum BUFFER {
      // AUDIO
      MASTER_INPUT  = 0,
      
      MASTER_OUT_L,
      MASTER_OUT_R,
      
      JACK_SEND_OUT,
      JACK_MASTER_OUT_L,
      JACK_MASTER_OUT_R,
      JACK_SIDECHAIN_KEY,
      JACK_SIDECHAIN_SIGNAL,
      
      SEND,
      SIDECHAIN_KEY,
      SIDECHAIN_SIGNAL,
      
      MASTER_RETURN_L,
      MASTER_RETURN_R,
      
      // MIDI
      MASTER_MIDI_INPUT,
      
      // track buffers: they are the "working" buffers per track:
      // the end result is mixed into the master output, while each
      // stage along the way the amplitude etc can be analysed
      TRACK_0,
      TRACK_1,
      TRACK_2,
      TRACK_3,
      TRACK_4,
      TRACK_5,
      TRACK_6,
      TRACK_7,
    };
    
    // Jack details
    jack_nframes_t         nframes;
    jack_nframes_t         samplerate;
    
    jack_nframes_t          transportFrame;
    jack_position_t*        transportPosition;
    jack_transport_state_t* transportState;
    
};

#endif // LUPPP_BUFFERS_H

