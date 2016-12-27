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

#ifndef LUPPP_STATE_H
#define LUPPP_STATE_H

#include <vector>

#include "stately.hxx"

/** State
 * This class keeps a list of all Stately instances, and calls save on them
 * when save() is called on this class.
**/
class State
{
public:
	State();

	/// called to initiate a save on all Statelys
	void save();

	/// called to initiate a reset on all Statelys
	void reset();

	/// called when each part is finished, and we can flush the metadata to disk
	void finish();

	void registerStately(Stately* s);

	int getNumStatelys();

private:
	std::vector<Stately*> statelys;
};

#endif // LUPPP_STATE_H
