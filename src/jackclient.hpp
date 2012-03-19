
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    
    void  apcWriteOutput(int track);
    void  apcWriteGridTrack(int track);
    void  apcWriteEffectParams(int track, int pos, bool active, float value);
    
    float* getHeadphonePflVector();
    float* getPostFaderSendVector();
    
    void writeMidi( void*, int,int,int);
    
    void deactivate();
    
    bool recordInput;
    
  private:
    Top* top;
    JClient* client;
    
    int nframes;
    
    // this PortBufferList represents float*'s that we can use to write audio
    CopyBufferList copyBufferList;
    
    Time time;
    Mixer mixer;
    
    JPort* sidechainPort;
    JPort* inputPort;
    JPort* outputPortW;
    JPort* outputPortX;
    JPort* outputPortY;
    JPort* outputPortZ;
    JPort* midiInputPort;
    
    JPort* trackInputPorts[8];
    
    JPort* headphonePflPort;
    
    JPort* monoPostFaderSend;
    JPort* bformatEffectReturnW;
    JPort* bformatEffectReturnX;
    JPort* bformatEffectReturnY;
    JPort* bformatEffectReturnZ;
    
    JPort* apcInputPort;
    JPort* apcOutputPort;
    
    JPort* seq24outputPort;
    
    int trackControlMode;
    
    float* headphonePflBuffer;
    float* postFaderSendBuffer;
    
    std::vector<float> headphonePflVector;
    std::vector<float> postFaderSendVector;
    
    void* seq24outputBuffer;
    void* apcOutputBuffer;
    void  apcRead(int);
    void  apcSendDeviceControl(int , int , void*);
    
    
    int processRtQueue();
    int process(jack_nframes_t);
    int processMidi(jack_nframes_t);
    
    int timebase(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos, void *arg);
    
    static int static_process(jack_nframes_t, void *);
    static int static_timebase(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos, void* thisPointer);
    
    int framesPerBeat;
    int nframeCounter;
    
    void* midiBuffer;
    void* midiInputBuffer;
};

#endif
