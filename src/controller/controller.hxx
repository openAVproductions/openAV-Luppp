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


#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

#include <string>
#include <jack/jack.h>

#include "../gridlogic.hxx"

/** Controller
 *  This is the base class for any device that interacts with Luppp.
 *  Overriding the virtual functions allows the subclass to be notified about
 *  that event type. To provide input, #include gridlogic.hxx and logic.hxx
**/
class Controller
{
public:
	/// used to report the controller status
	enum STATUS {
		CONTROLLER_ERROR = 0,
		CONTROLLER_OK,
	};

	/// creates the controller instance: this is called in the non-RT thread
	Controller();

	/// controllers have a unique ID
	int getID();

	/// allows instance to register MIDI observer type functionality
	virtual int registerComponents()
	{
		return 0;
	}

	virtual ~Controller() {};

	/// get the status of the controller
	virtual STATUS status()
	{
		return CONTROLLER_OK;
	}

	/// name string to show in UI
	virtual std::string getName() = 0;

	/// master
	virtual void masterInputVol(float f) {}
	virtual void masterInputTo(int to,float f) {}
	virtual void masterInputToActive(int to,float f) {}
	virtual void masterVolume(float f) {}
	virtual void masterReturnVolume(float f) {}
	virtual void metronomeEnable(bool b) {}

	/// FX
	virtual void trackSend(int t, int send, float v) {}
	virtual void trackSendActive(int t, int send, bool a) {}

	virtual void trackJackSend(int t, float v) {}
	virtual void trackJackSendActivate(int t, bool a) {}

	/// Time
	virtual void bpm(int bpm) {}
	virtual void tapTempo(bool b) {}

	/// Special
	virtual void specialScene(int t, int scene) {}

	/// track functionality
	virtual void pan(int t, float p) {}
	virtual void mute(int t, bool b) {}
	virtual void volume(int t, float f) {}
	virtual void progress(int t, int s, float f) {}
	virtual void recordArm(int t, bool r) {}
	virtual void setSceneState(int track, int scene, GridLogic::State s) {}
	virtual void launchScene( int scene ) {}

	/// reset controller
	virtual void reset();

	/// Jack MIDI (dis)connect
	virtual void midiConnect( jack_port_t* a, jack_port_t *b ) {}
	virtual void midiDisconnect( jack_port_t* a, jack_port_t *b ) {}

private:
	static int privateID;
	int ID;
};

#endif // LUPPP_CONTROLLER_H

