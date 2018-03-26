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

#ifndef LUPPP_LOGIC_H
#define LUPPP_LOGIC_H

#include "event.hxx"

/** Logic
 *  This class contains an interface exposing most functionality in Luppp. The
 *  interface is used to have one central place from where each controller can
 *  interact with Luppp using the same function calls.
 *
 *  This class should be used for input from any device. The interface is
 *  deliberatly specific with regards to scheduling events: controllers should
 *  NOT attempt to schedule changes: use the provided functions directly when
 *  events occur on a controller.
 *
 *  The ControllerUpdater class is the opposite of this class, it provides
 *  feedback of each event that occurs.
 *
 *  Note: The GridLogic class provides the interface to scene selection / state.
**/
class Logic
{
public:
	Logic();

	void tapTempo();
	void setBpm(float bpm); /// 0-1 input

	void metronomeEnable(bool b);

	void masterInputVol( float v );
	void masterInputTo( int inputTo, float v);
	void masterInputToActive( int inputTo, bool active);

	void masterReturn( int returnNum, float v);

	void trackPan(int track, float pan);
	void trackVolume(int track, float volume);
	void trackRecordArm(int track, bool armed);
	void trackSendActive(int track, int send, bool active);
	void trackSend(int track, int send, float value);

	void trackJackSendActivate(int t, bool active);
	void trackJackSend(int t, float vol);
	void looperUseAsTempo(int track, int scene);
	void looperClipLenght(int track, int scene, int lenght);
	void looperBarsToRecord(int track, int scene, int bars);
};

#endif // LUPPP_LOGIC_H
