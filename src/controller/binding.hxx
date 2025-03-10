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


#ifndef LUPPP_BINDING_H
#define LUPPP_BINDING_H

#include <map>

/// a LupppAction represents the Event type, as from Event.hxx
typedef int LupppAction;

class Binding
{
public:
	Binding() : status(0), data(0), echo(1), action(0), active(1),
		track(-2),scene(-1),send(-1)
	{
		ID = privateID++;
	}

	int ID;

	unsigned char status;
	unsigned char data;
	unsigned char echo;

	/// the action this binding relates to: this is an integer based on the
	/// event.hxx enumeration of event types
	LupppAction action;

	/// arguments to the event: track number, scene number etc
	int active;
	int track;
	int scene;
	int send;

	/// maps from Gridlogic::State to MIDI output value from binding
	std::map<int,int> clipStateMap;

private:
	/// static int counter to allow deleting this specific instance
	static int privateID;

};

#endif
