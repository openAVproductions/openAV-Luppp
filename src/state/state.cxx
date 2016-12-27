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

#include "state.hxx"

#include <iostream>
#include "../event.hxx"
#include "../eventhandler.hxx"

#include "../jack.hxx"

extern Jack* jack;

using namespace std;

State::State()
{
}

void State::registerStately(Stately* s)
{
	statelys.push_back( s );
}

void State::save()
{
	for( unsigned int i = 0; i < statelys.size(); i++) {
		statelys.at(i)->save();
	}
}

void State::reset()
{
	for( unsigned int i = 0; i < statelys.size(); i++) {
		statelys.at(i)->reset();
	}

	jack->getGridLogic()->updateState();
}

void State::finish()
{
	// trigger the GUI to write the metadata to disk, as each component of the
	// engine is done saving

	EventStateSaveFinish e;
	writeToGuiRingbuffer( &e );

}

int State::getNumStatelys()
{
	return statelys.size();
}
