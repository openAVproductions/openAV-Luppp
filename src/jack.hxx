/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    Jack(std::string name);
    ~Jack();
    
    static void setup(std::string name);
    
    void activate();
    /// quits the JACK client, destroying ports etc. Call only on exit of Luppp.
    void quit();
    
    int getBuffersize();
    int getSamplerate();
    
    // Luppp process callback: bar() events can occur between these
    void processFrames(int nframes);

    //Sets the first nframes of all the internal output buffers to zero. NO LIMIT CHECKS
    void clearInternalBuffers(int nframes);
    
    /// get functions for components owned by Jack 
    Looper*             getLooper(int t);
    TrackOutput*        getTrackOutput(int t);
    
    State*              getState(){return state;}
    Logic*              getLogic(){return logic;}
    Metronome*          getMetronome(){return metronome;}
    GridLogic*          getGridLogic(){return gridLogic;}
    TimeManager*        getTimeManager(){return timeManager;}
    ControllerUpdater*  getControllerUpdater(){return controllerUpdater;}

    void transportRolling(bool rolling);
    
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
    
    void resetMidiBindingState();
    
    // MIDI binding creation
    bool bindingEventRecordEnable;
    int bindingEventType;
    int bindingTrack;
    int bindingScene;
    int bindingSend;
    int bindingActive;
  
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
    /// _toMasterLag is a volume that lags behind _toMaster when setMaster() is called
    /// This prohibits audible jumps when rapidly changing the volume
    float masterVol;
    float masterVolLag;
    float masterVolDiff;
    float returnVol;
    
    float inputToMixVol;
    float inputToSendVol;
    float inputToXSideVol;
    
    bool  inputToKeyEnable;
    bool  inputToMixEnable;
    bool  inputToSendEnable;
    
    // JACK member variables
    bool clientActive;
    int samplerate;
    
    jack_port_t*  masterInput;
    jack_port_t*  masterOutputL;
    jack_port_t*  masterOutputR;
    jack_port_t*  masterReturnL;
    jack_port_t*  masterReturnR;
    jack_port_t*  headphonesPort;
    
    jack_port_t*  sidechainKeyOutput;
    jack_port_t*  sidechainSignalOutput;
    jack_port_t*  sendOutput;
    
    jack_port_t*  masterMidiInput;

    vector<jack_port_t*> trackJackOutputPorts;
    
    // JACK callback
    int  process (jack_nframes_t);
    
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

