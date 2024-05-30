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
class JackSendReturn;
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
	int setBufferSizeCallback();
	static int bufferSizeCallback(jack_nframes_t nframes, void *arg);

	// Luppp process callback: bar() events can occur between these
	void processFrames(int nframes);

	//Sets the first nframes of all the internal output buffers to zero. NO LIMIT CHECKS
	void clearInternalBuffers(int nframes);

	/// get functions for components owned by Jack
	Looper*             getLooper(int t);
	TrackOutput*        getTrackOutput(int t);

	State*              getState()
	{
		return state;
	}
	Logic*              getLogic()
	{
		return logic;
	}
	Metronome*          getMetronome()
	{
		return metronome;
	}
	GridLogic*          getGridLogic()
	{
		return gridLogic;
	}
	TimeManager*        getTimeManager()
	{
		return timeManager;
	}
	ControllerUpdater*  getControllerUpdater()
	{
		return controllerUpdater;
	}
	
	float getInputVolume() 
	{
		return inputVol;
	}

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

	JackSendReturn *getJackSendReturn(int t);

	float smoothing_value;
private:
	int                 lastnframes;
	jack_client_t*      client;

	Buffers             buffers;
	TimeManager*        timeManager;
	Metronome*          metronome;
	State*              state;
	Logic*              logic;
	GridLogic*          gridLogic;
	ControllerUpdater*  controllerUpdater;

	vector<Looper*>         loopers;
	vector<JackSendReturn*> tracksendreturns;
	vector<TrackOutput*>    trackOutputs;

	vector<MidiIO*>   midiIO;

	// FX
	DBMeter* inputMeter;
	DBMeter* masterMeter;
	
	/// *Lag are values that lag behind their corresponding value
	/// This prohibits audible jumps when rapidly changing the volume
	float inputVol;
	float inputVolLag;
	
	float masterVol;
	float masterVolLag;

	float returnVol;
	float returnVolLag;

	float inputToMixVol;
	float inputToMixVolLag;

	float inputToSendVol;
	float inputToSendVolLag;

	float inputToXSideVol;
	float inputToXSideVolLag;

	bool  inputToKeyEnable;
	float inputToKeyEnableLag;

	bool  inputToMixEnable;
	float inputToMixEnableLag;

	bool  inputToSendEnable;
	float inputToSendEnableLag;

	// JACK member variables
	bool clientActive;
	int samplerate;

	jack_port_t*  masterInputL;
	jack_port_t*  masterInputR;
	jack_port_t*  masterOutputL;
	jack_port_t*  masterOutputR;

	jack_port_t*  masterReturnL;
	jack_port_t*  masterReturnR;
	jack_port_t*  headphonesPortL;
	jack_port_t*  headphonesPortR;

	jack_port_t*  sidechainKeyOutputL;
	jack_port_t*  sidechainKeyOutputR;
	jack_port_t*  sidechainSignalOutputL;
	jack_port_t*  sidechainSignalOutputR;
	jack_port_t*  sendOutputL;
	jack_port_t*  sendOutputR;

	jack_port_t*  masterMidiInput;

	jack_port_t* trackJackOutputPorts[NTRACKS*2];

	// JACK callback
	int  process (jack_nframes_t);

	int timebase(jack_transport_state_t,
	             jack_nframes_t,
	             jack_position_t*,
	             int );

	void connect(jack_port_id_t, jack_port_id_t, int);

	// static JACK callbacks
	static int static_process  (jack_nframes_t, void *);

	static int static_timebase (jack_transport_state_t,
	                            jack_nframes_t,
	                            jack_position_t*,
	                            int,
	                            void* );

	static void static_connect (jack_port_id_t, jack_port_id_t, int, void *);

	// UI update variables
	int uiUpdateCounter;
	int uiUpdateConstant;

};

#endif // LUPPP_JACK_H

