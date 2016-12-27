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

#include "stately.hxx"

#include "../jack.hxx"
#include "../eventhandler.hxx"
#include "state.hxx"

extern Jack* jack;

int Stately::saveSuccess = 0;
int Stately::saveErrors  = 0;

Stately::Stately()
{
	jack->getState()->registerStately( this );
}


void Stately::reset()
{
}

void Stately::save()
{
}

void Stately::checkCompletedSave()
{
	if ( (saveSuccess + saveErrors) >= jack->getState()->getNumStatelys() ) {
		jack->getState()->finish();

		if ( saveErrors ) {
			// send message to UI to be printed, noting # of clips unsaved due to errors
			char buf[50];
			sprintf( buf, "Saved with %i clips !saved due to errors", saveErrors);
			EventGuiPrint e( buf );
			writeToGuiRingbuffer( &e );
		}

		// reset in case of another save before quit
		saveErrors  = 0;
		saveSuccess = 0;
	}
}

void Stately::success()
{
	saveSuccess++;
	checkCompletedSave();
}

void Stately::error(const char* errorString)
{
	// CRITICAL FIXME: add error handling code, noting an error occured, perhaps prompt user?
	saveErrors++;
	checkCompletedSave();
}
