
#ifndef LUPPP_JACK_CLIENT
#define LUPPP_JACK_CLIENT

#include <jack/jack.h>
#include <jack/types.h>
#include <jack/midiport.h>
#include <jack/transport.h>

#include <vector>
#include <sndfile.hh>

#include "mixer.hpp"

#include "top.hpp"

typedef jack_client_t JClient;
typedef jack_port_t JPort;

enum ApcTrackControlMode {
  APC_TRACK_CONTROL_MODE_PAN    = 87,
  APC_TRACK_CONTROL_MODE_SEND_A = 88,
  APC_TRACK_CONTROL_MODE_SEND_B = 89,
  APC_TRACK_CONTROL_MODE_SEND_C = 90,
};

class JackClient
{
  public:
    JackClient(Top*);
    
    void deactivate();
    
  private:
    Top* top;
    JClient* client;
    
    Mixer mixer;
    
    JPort* sidechainPort;
    JPort* inputPort;
    JPort* outputPort;
    JPort* midiInputPort;
    
    JPort* apcInputPort;
    JPort* apcOutputPort;
    
    int trackControlMode;
    void apcRead(int);
    
    void writeMidi( void*, int,int,int);
    
    int process(jack_nframes_t);
    int processMidi(jack_nframes_t);
    
    static int static_process(jack_nframes_t, void *);
    
    int framesPerBeat;
    int nframeCounter;
    
    void* midiBuffer;
    void* midiInputBuffer;
};

#endif
