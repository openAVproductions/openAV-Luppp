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

#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/controller.hxx"

#include "gridlogic.hxx"

using namespace std;


/** ControllerUpdater
 *  Updates each registered controller when a certain event occurs. All output
 *  devices (MIDI controllers, GUI, OSC-UI's etc) are registered in order to
 *  stay up-to-date.
 *
 *  This class does no scheduling, it passes the events to the Controllers
 *  immidiatly.
 *
 *  The Logic class is the opposite of this: it takes input and Luppp processes
 *  it, pushing the relevant updates in state through ControllerUpdater to each
 *  registered device.
**/
class ControllerUpdater
{
public:
	ControllerUpdater();

	/// add a Controller* to Controllers that recieve updates
	void registerController( Controller* controller );

	/// removes a controller by ID
	void removeController( int controllerID );

	/// returns a Controller* from ID
	Controller* getController(int id);

	void reset();
	void mute(int t, bool b);

	void masterVolume(float v);
	void masterReturnVolume(float v);
	void masterInputToActive(int to, bool v);
	void masterInputTo( int inputTo, float vol );
	void masterInputVol( float vol );

	void setTrackSceneProgress(int t, int s, float p);
	void setTrackSendActive(int t, int send, bool v);
	void setTrackSend(int t, int send, float v);

	void setTrackJackSendActive(int t, bool v);
	void setTrackJackSend(int t, float v);

	void specialScene(int t, int scene);

	void launchScene( int scene );

	void setSceneState(int t, int clip, GridLogic::State s);

	void recordArm(int t, bool r);

	void volume(int t, float v);

	void tapTempo(bool b);

	void metronomeEnable(bool b);

private:
	std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

