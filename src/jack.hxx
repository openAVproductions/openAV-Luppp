
#ifndef LUPPP_JACK_H
#define LUPPP_JACK_H

// Library
#include <vector>
#include <cstring>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/transport.h>

#include "event.hxx"

class MidiIO;

class Reverb;
class DBMeter;

class State;
class Logic;
class Looper;
class Metronome;
class GridLogic;
class TimeManager;
class ControllerUpdater;

class TrackOutput;

// INPUT_TO
//#include "gridlogic.hxx"

using namespace std;

/** Jack
  This code contains the JACK client.
  It allows reading / writing of audio / midi.
**/
class Jack
{
  public:
    Jack();
    ~Jack();
    
    void activate();
    /// quits the JACK client, destroying ports etc. Call only on exit of Luppp.
    void quit();
    
    int getBuffersize();
    int getSamplerate();
    
    /// get functions for components owned by Jack 
    Looper*             getLooper(int t);
    TrackOutput*        getTrackOutput(int t);
    
    State*              getState(){return state;}
    Logic*              getLogic(){return logic;}
    Metronome*          getMetronome(){return metronome;}
    GridLogic*          getGridLogic(){return gridLogic;}
    TimeManager*        getTimeManager(){return timeManager;}
    ControllerUpdater*  getControllerUpdater(){return controllerUpdater;}
    
    /// register a MIDI observer
    void registerMidiIO( MidiIO* mo );
    void unregisterMidiIO( MidiIO* mo );
    
    /// set the master i/o volume / sends
    void inputVolume( float vol );
    void masterVolume( float vol );
    void returnVolume( float vol );
    void inputTo(INPUT_TO to, float v);
    void inputToActive(INPUT_TO to, bool a);
    
    jack_client_t* getJackClientPointer()
    {
      return client;
    };
    
    // MIDI binding creation
    bool bindingEventRecordEnable;
    int bindingTrack;
    int bindingEventType;
  
  private:
    jack_client_t* client;
    
    Buffers             buffers;
    TimeManager*        timeManager;
    Metronome*          metronome;
    State*              state;
    Logic*              logic;
    GridLogic*          gridLogic;
    ControllerUpdater*  controllerUpdater;
    
    vector<Looper*>         loopers;
    vector<TrackOutput*>    trackOutputs;
    
    vector<MidiIO*>   midiIO;
    
    // FX
    DBMeter* inputMeter;
    DBMeter* masterMeter;
    
    float inputVol;
    float masterVol;
    float returnVol;
    
    float inputToMixVol;
    float inputToSendVol;
    float inputToXSideVol;
    
    bool  inputToKeyEnable;
    bool  inputToMixEnable;
    bool  inputToSendEnable;
    
    // JACK member variables
    bool clientActive;
    
    jack_port_t*  masterInput;
    jack_port_t*  masterOutputL;
    jack_port_t*  masterOutputR;
    jack_port_t*  masterReturnL;
    jack_port_t*  masterReturnR;
    
    jack_port_t*  sidechainKeyOutput;
    jack_port_t*  sidechainSignalOutput;
    jack_port_t*  sendOutput;
    
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

