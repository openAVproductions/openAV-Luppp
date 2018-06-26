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

#include "mappa.hxx"

#include "config.hxx"
#include "eventhandler.hxx"

#include <ctlra/mappa.h>

#include <errno.h>
#include <iostream>
#include <sys/stat.h>

Mappa::Mappa()
{
	printf("mappa()\n");
	mappa = mappa_create(NULL);
	if(!mappa)
		printf("Mappa() error return from mappa_create()\n");
}

void target_track_vol(uint32_t target_id, float value, void *token,
		      uint32_t token_size, void *userdata)
{
	uint32_t *track = (uint32_t *)token;
	EventTrackVol e(*track, value);
	writeToDspRingbuffer( &e );
}

void target_track_send(uint32_t target_id, float value, void *token,
		       uint32_t token_size, void *userdata)
{
	uint32_t *track = (uint32_t *)token;
	EventTrackSend e(*track, SEND_POSTFADER, value);
	writeToDspRingbuffer( &e );
}

int Mappa::register_targets()
{
	/* TODO: register some targets here */
	printf("registering targets now\n");

	struct mappa_target_t t = {0};
	t.userdata = this;

	const char *track_targets[] = {
		"vol",
		"send",
	};
	mappa_target_float_func funcs[] = {
		target_track_vol,
		target_track_send,
	};

	for(int j = 0; j < 2; j++) {
		for(int i = 0; i < NTRACKS; i++) {
			char buf[32];
			snprintf(buf, sizeof(buf), "luppp:track_%d_%s", i,
				 track_targets[j]);
			t.name = buf;
			t.func = funcs[j];
			/* add a target with the "token payload" as the value of
			 * i. This will be passed back when the callback is invoked.
			 * This allows the callback to easily identify which track
			 * it is that this target is pointing to */
			int32_t ret = mappa_target_add(mappa, &t, 0,
						       &i, sizeof(int));
			if(ret)
				printf("error registering target %s\n", t.name);
		}
	}

	int32_t ret = mappa_load_bindings(mappa, "z1_mappa.ini");
	printf("load bindings ret = %d\n", ret);

	return 0;
}

int Mappa::iter()
{
	mappa_iter(mappa);
	return 0;
}

Mappa::~Mappa()
{
	mappa_destroy(mappa);
}

