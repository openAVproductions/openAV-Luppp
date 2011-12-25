
#ifndef LUPPP_JACK_CLIENT
#define LUPPP_JACK_CLIENT

#include <jack/jack.h>
#include <jack/types.h>
#include <jack/midiport.h>
#include <jack/transport.h>

#include <vector>
#include <sndfile.hh>

#include "time.hpp"
#include "mixer.hpp"

#include "controller.hpp"

#include "porttypes.hpp"

class Top;

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
    
    void* getApcOutputBuffer();
    void writeMidi( void*, int,int,int);
    
    void deactivate();
    
    bool recordInput;
    
  private:
    Top* top;
    JClient* client;
    
    int nframes;
    
    // this PortBufferList represents float*'s that we can use to write audio
    PortBufferList portBufferList;
    
    Time time;
    Mixer mixer;
    
    JPort* sidechainPort;
    JPort* inputPort;
    JPort* outputPortW;
    JPort* outputPortX;
    JPort* outputPortY;
    JPort* outputPortZ;
    JPort* midiInputPort;
    
    JPort* apcInputPort;
    JPort* apcOutputPort;
    
    int trackControlMode;
    
    void* apcOutputBuffer;
    void  apcRead(int);
    void  apcSendDeviceControl(int , int , void*);
    
    int processRtQueue();
    int process(jack_nframes_t);
    int processMidi(jack_nframes_t);
    
    static int static_process(jack_nframes_t, void *);
    
    int framesPerBeat;
    int nframeCounter;
    
    void* midiBuffer;
    void* midiInputBuffer;
};

#endif
