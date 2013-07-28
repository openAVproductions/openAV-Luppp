
#ifndef LUPPP_JACK_H
#define LUPPP_JACK_H

/*
    jack.hxx
  
  This code contains the JACK client.
  It allows reading / writing of audio / midi.
  
*/

// Library
#include <vector>
#include <cstring>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/transport.h>

#include "config.hxx"
#include "looper.hxx"
#include "trackoutput.hxx"
#include "metronome.hxx"
#include "timemanager.hxx"

#include "dsp/dsp_reverb.hxx"
#include "dsp/dsp_dbmeter.hxx"

#include "controllerupdater.hxx"

using namespace std;

class Jack
{
  public:
    Jack();
    
    void activate();
    int getBuffersize();
    int getSamplerate();
    
    void setLooperState(int t, int sc, Looper::State s)
    {
      loopers.at(t)->setState(s);
      loopers.at(t)->setScene(sc);
    }
    void setLooperLoopLength(int t, float l)
    {
      loopers.at(t)->setLoopLength(l);
    }
    Looper* getLooper(int t)
    {
      return loopers.at(t);
    }
    TrackOutput* getTrackOutput(int t)
    {
      return trackOutputs.at(t);
    }
    
    Metronome*   getMetronome(){return &metronome;}
    TimeManager* getTimeManager(){return &timeManager;}
    ControllerUpdater* getControllerUpdater(){return &controllerUpdater;}
    
    void writeApcOutput( unsigned char* data );
  
  private:
    Buffers     buffers;
    Metronome   metronome;
    TimeManager timeManager;
    
    ControllerUpdater controllerUpdater;
    
    vector<Looper*> loopers;
    
    vector<TrackOutput*> trackOutputs;
    
    int nframes;
    int samplerate;
    
    // FX
    Reverb* reverb;
    DBMeter* reverbMeter;
    DBMeter* masterMeter;
    
    // JACK member variables
    jack_client_t* client;
    
    jack_port_t*  masterInput;
    jack_port_t*  masterOutput;
    
    jack_port_t*  apcMidiInput;
    jack_port_t*  apcMidiOutput;
    jack_port_t*  masterMidiInput;
    
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
    
    // UI update variables
    int uiUpdateCounter;
    int uiUpdateConstant;
};

#endif // LUPPP_JACK_H

