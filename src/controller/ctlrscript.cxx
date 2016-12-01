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

#include "../jack.hxx"
#include "../event.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"

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
	uint32_t size;
};


CtlrScript::CtlrScript(std::string filename) :
	Controller()
{
	printf("%s, attempting to compile %s\n", __func__, filename.c_str());
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
		return;
	}

	program = malloc(tcc_relocate(s, NULL));
	if(!program)
		error("failed to alloc mem for program\n");
	ret = tcc_relocate(s, program);
	if(ret < 0)
		error("failed to relocate code to program memory\n");

	poll = (ctlr_poll)tcc_get_symbol(s, "d2_poll");
	if(!poll)
		error("failed to get de poll\n");

	handle = (void (*)(void*))tcc_get_symbol(s, "d2_handle");
	if(!handle)
		error("failed to get d2 handle\n");

	tcc_delete(s);

	uint32_t iter = 0;
	iter = poll(iter);

	struct event_t ev = { 0, 1 };
	if(iter == 1)
		ev.type = 1;
	handle(&ev);

#warning FIXME: Free program when needed
	//free(program);
}

void CtlrScript::bpm(int bpm)
{
	printf("%s : %d\n", __func__, bpm);
}
