
#ifndef LUPPP_BUFFERS_H
#define LUPPP_BUFFERS_H

#include <cstring>
#include <jack/transport.h>

class Buffers
{
  public:
    Buffers()
    {
      for(int i = 0; i < 32; i++)
      {
        audio[i] = 0;
        midi [i] = 0;
      }
      //memset( audio, 0, sizeof(float*)*32);
      //memset( midi , 0, sizeof(void *)*32);
    }
    float* audio[32];
    void*  midi [32];
    
    enum BUFFER {
      // AUDIO
      MASTER_INPUT  = 0,
      MASTER_OUTPUT = 1,
      JACK_MASTER_OUTPUT = 2,
      
      REVERB = 3,
      SIDECHAIN = 4,
      POST_SIDECHAIN = 5,
      
      // MIDI
      MASTER_MIDI_INPUT  = 6,
      APC_INPUT = 7,
      APC_OUTPUT = 8,
      
      // track buffers: they are the "working" buffers per track:
      // the end result is mixed into the master output, while each
      // stage along the way the amplitude etc can be analysed
      TRACK_0 = 9,
      TRACK_1 = 10,
      TRACK_2 = 11,
      TRACK_3 = 12,
      TRACK_4 = 13,
      TRACK_5 = 14,
      TRACK_6 = 15,
      TRACK_7 = 16,
    };
    
    // Jack details
    jack_nframes_t         nframes;
    jack_nframes_t         samplerate;
    
    jack_nframes_t          transportFrame;
    jack_position_t*        transportPosition;
    jack_transport_state_t* transportState;
    
};

#endif // LUPPP_BUFFERS_H

