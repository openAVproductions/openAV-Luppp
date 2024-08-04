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


#ifndef LUPPP_GENERIC_MIDI_H
#define LUPPP_GENERIC_MIDI_H

#include "controller.hxx"

#include <string>
#include <vector>

#include "../cjson/cJSON.h"
#include "../observer/midi.hxx"

#include "binding.hxx"

/** GenericMIDI
 *  This class is used to load a <controller>.cfg JSON file as a MIDI map.
 *  The name parameter is the name of the JACK MIDI ports that are registered.
**/
class GenericMIDI : public Controller, public MidiIO
{
public:
	/// Creates a blank GenericMIDI instance, which can be bound dynamically
	GenericMIDI(int waste = 0, std::string name = "generic");

	/// Attempts to load a .ctlr file, pointed to by the string
	GenericMIDI(std::string file);

	int registerComponents();

	Controller::STATUS status();

	void setName(std::string );
	std::string getName();
	std::string getAuthor();
	std::string getEmail();

	void metronomeEnable(bool b);

	void launchScene( int scene );

	void volume(int t, float f);


	void recordArm(int t, bool b);
	void setSceneState(int track, int clip, GridLogic::State s);

	void trackSend(int t, int send, float v);
	void trackSendActive(int t, int send, bool a);

	virtual void trackJackSend(int t, float v);
	virtual void trackJackSendActivate(int t, bool a);

	/// footswitch -> scene launch controls
	void setFootswitchToNextScene(int v);
	bool footswitchNextScene;
	bool footswitchPrevScene;

	virtual void setBarBeat(int ba, int be);


	void reset();

	void midi(unsigned char* data);

	void process(int nframes);

	const std::vector<Binding*>& getMidiToAction();

	// for adding bindings from MIDI / GUI event pair
	void setupBinding( LupppAction eventType, int midiStatus, int midiData, int track, int scene, int send, int active );
	void removeBinding( int bindingID );

	void midiConnect(jack_port_t* a, jack_port_t* b);

private:
	STATUS stat;

	std::string name;
	std::string author;
	std::string email;

	/// contains midi binding instances
	std::vector<Binding*> midiToAction;
	std::vector<Binding*> actionToMidi;

	int loadController(std::string controllerFile);

	/// creates a binding from a cJSON inputBindings / outputBindings object
	Binding* setupBinding( cJSON* bindings );


	/// for "sampling" a clip in the grid, and applying events to it:
	/// footpedal for example
	bool shiftPressed;
	int footpedalTrack;
	int footpedalScene;

	/*
	/// for handling events
	void ccChange( int track, int cc, float value );
	void noteOff( int track, int note, int vel );
	void noteOn( int track, int note, int vel );
	*/
};

#endif // LUPPP_GENERIC_MIDI_H

