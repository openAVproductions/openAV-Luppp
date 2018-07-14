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


#ifndef LUPPP_CTLRA_H
#define LUPPP_CTLRA_H

#include "controller.hxx"

#include <ctlra/ctlra.h>
#include <ctlra/event.h>

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
/* Handle incoming events from Ctlra controllers */
typedef int (*tcc_event_func_t)(int num, struct ctlra_event_t **);
/* Handle incoming Luppp events - and send responses to controller */
typedef void (*ctlra_handle_event)(void *ctlra, enum EVENT_ID id, void *);

class CtlraScript : public Controller, public MidiIO
{
public:
	CtlraScript(std::string filename);
	virtual ~CtlraScript();

	/* called to register this instance with JACK */
	int registerComponents();

	std::string getName()
	{
		return "CtlraScript";
	}

	virtual void poll();

	const char *getCtlraName(int type, int id);

	void barBeat(int bar, int beat);
	void trackSendActive(int t, int send, bool a);
	void setSceneState(int track, int scene, GridLogic::State s);

	void poll_devices();/* for ctlra poll() */
	void midi(unsigned char* midi);
	void ctlra_forward(struct ctlra_dev_t* dev, uint32_t num_events,
			   struct ctlra_event_t** events);
	void writeCtlraLight(int light,int status);

	Controller::STATUS status();

private:
	/* a flag stating if the program linked OK. If not, we do not act
	 * on the script, and do not call *any* function pointers related
	 * to the script, as they *will* cause a segfault */
	int program_ok;

	void *program;
	tcc_event_func_t tcc_event_func;
	ctlra_handle_event handle;

	std::string filename;

	Controller::STATUS stat;

	int compile();
	void script_reload();

	/* Ctlra stuff */
	struct ctlra_t* dev;
	void* hack_current_dev;

	/* For tracking when to re-load the script file, this allows
	 * taking the modified time, and recompiling when needed */
	time_t script_load_time;
};

#endif // LUPPP_CTLRA_H

