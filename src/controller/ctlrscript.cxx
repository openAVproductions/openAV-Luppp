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

#include "ctlrscript.hxx"

#include <errno.h>
#include <iostream>
#include <sys/stat.h>

#include "../jack.hxx"
#include "../event.hxx"
#include "../logic.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"
#include "luppp_script_api.h"

#include "libtcc.h"

extern Jack* jack;

static void error_func(void *opaque, const char *msg)
{
	printf("TCC ERROR: %s\n", msg);
}

static void error(const char *msg)
{
	printf("%s\n", msg);
}

void luppp_do(enum EVENT_ID id, void* e)
{
	printf("%s : event: %d, %p\n", __func__, id, e);
	switch(id) {
	case EVENT_TRACK_SEND_ACTIVE: {
		struct event_track_send_active *ev =
			(struct event_track_send_active *)e;
		jack->getLogic()->trackSendActive(ev->track, ev->send,
						  ev->active);
		break;
		}
	case EVENT_TRACK_SEND: {
		struct event_track_send *ev =
			(struct event_track_send *)e;
		jack->getLogic()->trackSend(ev->track, ev->send,
						  ev->value);
		break;
		}
	default: break;
	}
}

static int file_modify_time(const char *path, time_t *new_time)
{
	if(new_time == 0)
		return -1;

	struct stat file_stat;
	int err = stat(path, &file_stat);
	if (err != 0) {
		return -2;
	}
	*new_time = file_stat.st_mtime;
	return 0;
}

int CtlrScript::compile()
{
	if(program) {
		free(program);
	}
	program_ok = 0;

	TCCState *s;
	s = tcc_new();
	if(!s)
		error("failed to create tcc context\n");

	tcc_set_error_func(s, 0x0, error_func);
	tcc_set_options(s, "-g");
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	int ret = tcc_add_file(s, filename.c_str());
	if(ret < 0) {
		printf("gracefully handling error now... \n");
		return -EINVAL;
	}

	tcc_add_symbol(s, "luppp_do", (void *)luppp_do);
	if(ret < 0) {
		error("failed to insert luppp_do() symbol\n");
		return -EINVAL;
	}

	program = malloc(tcc_relocate(s, NULL));
	if(!program)
		error("failed to alloc mem for program\n");
	ret = tcc_relocate(s, program);
	if(ret < 0) {
		error("failed to relocate code to program memory\n");
		return -EINVAL;
	}

	poll = (ctlr_handle_midi)tcc_get_symbol(s, "d2_poll");
	if(!poll) {
		error("failed to get de poll\n");
		return -EINVAL;
	}

	/* handles events from Luppp */
	handle = (ctlr_handle_event)tcc_get_symbol(s, "d2_handle");
	if(!handle)
		error("failed to get d2 handle\n");

	tcc_delete(s);

	/* Store the time of compiling */
	file_modify_time(filename.c_str(), &script_load_time);
	program_ok = 1;
}


void CtlrScript::script_reload()
{
	time_t new_time;
	int err = file_modify_time(filename.c_str(), &new_time);
	if(err)
		return;

	if(new_time > script_load_time) {
		printf("CtlrScript %d : Reloading script \"%s\"\n",
		       getID(), filename.c_str());
		compile();
	}
}


CtlrScript::CtlrScript(std::string file) :
	Controller(),
	MidiIO()
{
	printf("%s, attempting to compile %s\n", __func__, file.c_str());

	this->filename = file;

	int err = compile();
	if(err)
		return;

	stat = CONTROLLER_OK;
}

int CtlrScript::registerComponents()
{
	// makes JACK add this controller to the midiObservers list:
	// note the static_cast<>() is *needed* here for multiple-inheritance
	MidiIO* m = static_cast<MidiIO*>(this);

	registerMidiPorts( "testScripted" );

	jack->registerMidiIO( m );

	return LUPPP_RETURN_OK;
}


Controller::STATUS CtlrScript::status()
{
	return stat;
}

void CtlrScript::midi(unsigned char* midi)
{
	script_reload();

	if(!program_ok)
		return;

	int status  = midi[0];
	int data    = midi[1];
	float value = midi[2] / 127.f;

	printf("%s : got %2x %2x %0.2f\n", __func__, status, data, value);
	int ret = poll(midi);
}

struct event_trac_send_active_t {
	int type;
	int track;
	int send;
	int active;
};

void CtlrScript::trackSendActive(int t, int send, bool a)
{
	if(!program_ok)
		return;

	printf("%s : %d : %d\n", __func__, send, a);
	struct event_trac_send_active_t ev = {
		.type = 3,
		.track = t,
		.send = send,
		.active = 0,
	};
	if(a)
		ev.active = 1;
	printf("ev active = %d\n", ev.active);
	handle(&ev);
}
