/*
 * Author: Harry van Haaren 2018
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


#ifndef LUPPP_MAPPA_H
#define LUPPP_MAPPA_H

#include <ctlra/ctlra.h>
#include <ctlra/event.h>
#include <ctlra/mappa.h>

/** This is the Mappa implementation for Luppp. */
class Mappa
{
public:
	Mappa();
	virtual ~Mappa();

	/* initialize the luppp software targets which can be modified
	 * by Mappa. This allows the user to create mappings from their
	 * hardware to these Luppp control knobs. */
	int register_targets();

	/* iterate the mappings - poll devices and handle I/O. Performs
	 * the target callbacks in this thread */
	int iter();

private:
	struct mappa_t *mappa;
};

#endif // LUPPP_MAPPA_H

