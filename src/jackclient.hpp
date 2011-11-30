
#ifndef LUPPP_JACK_CLIENT
#define LUPPP_JACK_CLIENT

#include <jack/jack.h>
#include <jack/types.h>
#include <jack/midiport.h>
#include <jack/transport.h>

#include <sndfile.hh>
#include <vector>

typedef jack_client_t JClient;
typedef jack_port_t JPort;

class JackClient
{
  public:
    JackClient();
    
    void deactivate();
    
  private:
    static JClient* client;
    
    static JPort* sidechainPort;
    static JPort* inputPort;
    static JPort* outputPort;
    static JPort* midiInputPort;
    
    static int process(jack_nframes_t, void *);
    
    static int framesPerBeat;
    static int nframeCounter;
    
    static void writeMidi(int,int,int);
    static void* midiBuffer;
    static void* midiInputBuffer;
};

#endif
