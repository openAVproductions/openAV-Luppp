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

#include <iostream>
#include <errno.h>

#include "../jack.hxx"
#include "../event.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"
#include <sys/stat.h>

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

#warning TODO: Externalize this to a header which the controllers should\
include to understand the event types, and functions for sending
struct event_t {
	uint32_t type;
};

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

int CtlrScript::compile(const char* filename)
{
#warning FIXME: Free program when it exists - replace the other version\
	 Note this is the RT audio thread - so loading / replacing should\
	 be done in an offline thread.
	if(program) {
		free(program);
	}

	TCCState *s;
	s = tcc_new();
	if(!s)
		error("failed to create tcc context\n");

	tcc_set_error_func(s, 0x0, error_func);
	tcc_set_options(s, "-g");
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	int ret = tcc_add_file(s, filename);
	if(ret < 0) {
		printf("gracefully handling error now... \n");
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

	poll = (ctlr_poll)tcc_get_symbol(s, "d2_poll");
	if(!poll) {
		error("failed to get de poll\n");
		return -EINVAL;
	}

	handle = (void (*)(void*))tcc_get_symbol(s, "d2_handle");
	if(!handle)
		error("failed to get d2 handle\n");

	tcc_delete(s);

	/* Store the time of compiling */
	file_modify_time(filename, &script_load_time);

	uint32_t iter = 0;
	//iter = poll(iter);

	struct event_t ev = { 0 };
	if(iter == 1)
		ev.type = 1;
	handle(&ev);
}


CtlrScript::CtlrScript(std::string filename) :
	Controller(),
	MidiIO()
{
	printf("%s, attempting to compile %s\n", __func__, filename.c_str());

	int err = compile(filename.c_str());
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
	int status  = midi[0];
	int data    = midi[1];
	float value = midi[2] / 127.f;

	printf("%s : got %2x %2x %0.2f\n", __func__, status, data, value);
	int ret = poll(midi);
}

struct event_bpm_t {
	struct event_t event;
	int bpm;
};

void CtlrScript::bpm(int bpm)
{
	printf("%s : %d\n", __func__, bpm);
	//struct event_t ev = { 2, 1 };

	struct event_bpm_t ev = {
		.event = { .type = 2 },
		.bpm = bpm,
	};

	handle(&ev);
}

struct event_trac_send_active_t {
	struct event_t event;
	int track;
	int send;
	int active;
};

void CtlrScript::trackSendActive(int t, int send, bool a)
{
	printf("%s : %d : %d\n", __func__, send, a);
	struct event_trac_send_active_t ev = {
		.event = { .type = 3 },
		.track = t,
		.send = send,
		.active = 0,
	};
	if(a)
		ev.active = 1;
	printf("ev active = %d\n", ev.active);
	handle(&ev);
}
