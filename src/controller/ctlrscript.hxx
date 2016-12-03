/*
 * Author: Harry van Haaren 2016
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


#ifndef LUPPP_CONTROLLER_SCRIPT_H
#define LUPPP_CONTROLLER_SCRIPT_H

#include "controller.hxx"

#include "../observer/midi.hxx"
#include "luppp_script_api.h"

/** This is a controller using TCC as a backend, which reads .c files
 * as "scripts", but compiles them on the fly and then hooks in the
 * poll / handle event loop into the controller itself.
 *
 * It is designed to be a flexible way of prototyping support for any
 * MIDI/HID devices. Once a controller file is concidered "finished",
 * it may be sent as a pull request for inclusion in vanilla Luppp.
 *
 * As TCC is only a C compiler (as opposed to C++) this layer translates
 * Luppps internal events (which are C++ classes) to native C structs,
 * which can then be passed to the TCC compiled C backend. Ideally the
 * Luppp internal event handling system would be ported to plain C,
 * however this would also require significant effort, which does not
 * seem worth the time at the moment.
 */

/* These typedefs are for the poll and handle events of the scripted
 * controller, to receieve and send events as needed */
typedef int (*ctlr_handle_midi)(unsigned char *midi);
typedef void (*ctlr_handle_event)(enum EVENT_ID id, void *);

class CtlrScript : public Controller, public MidiIO
{
public:
	CtlrScript(std::string filename);

	/* called to register this instance with JACK */
	int registerComponents();

	std::string getName()
	{
		return "CtlrScript";
	}

	void trackSendActive(int t, int send, bool a);

	void midi(unsigned char* midi);

	Controller::STATUS status();

private:
	/* a flag stating if the program linked OK. If not, we do not act
	 * on the script, and do not call *any* function pointers related
	 * to the script, as they *will* cause a segfault */
	int program_ok;

	void *program;
	ctlr_handle_midi  poll;
	ctlr_handle_event handle;

	std::string filename;

	Controller::STATUS stat;

	int compile();
	void script_reload();

	/* For tracking when to re-load the script file, this allows
	 * taking the modified time, and recompiling when needed */
	time_t script_load_time;


};

#endif // LUPPP_CONTROLLER_SCRIPT_H

