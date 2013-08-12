
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
      
      MASTER_OUT_L = 1,
      MASTER_OUT_R = 2,
      
      JACK_MASTER_OUT_L = 3,
      JACK_MASTER_OUT_R = 4,
      
      REVERB = 5,
      SIDECHAIN = 6,
      POST_SIDECHAIN = 7,
      
      // MIDI
      MASTER_MIDI_INPUT = 8,
      APC_INPUT = 9,
      APC_OUTPUT = 10,
      
      // track buffers: they are the "working" buffers per track:
      // the end result is mixed into the master output, while each
      // stage along the way the amplitude etc can be analysed
      TRACK_0 = 11,
      TRACK_1 = 12,
      TRACK_2 = 13,
      TRACK_3 = 14,
      TRACK_4 = 15,
      TRACK_5 = 16,
      TRACK_6 = 17,
      TRACK_7 = 18,
    };
    
    // Jack details
    jack_nframes_t         nframes;
    jack_nframes_t         samplerate;
    
    jack_nframes_t          transportFrame;
    jack_position_t*        transportPosition;
    jack_transport_state_t* transportState;
    
};

#endif // LUPPP_BUFFERS_H

