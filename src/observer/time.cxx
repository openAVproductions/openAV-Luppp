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

#include "time.hxx"

#include "../audioengine.hxx"
extern AudioEngine* g_pAudioEngine;

#include "../timemanager.hxx"

TimeObserver::TimeObserver()
{
	// Register with TimeManager if g_pAudioEngine is available
	if (g_pAudioEngine && g_pAudioEngine != nullptr) {
		g_pAudioEngine->getTimeManager()->registerObserver( this );
	}
}
