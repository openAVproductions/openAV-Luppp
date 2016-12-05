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


#ifndef LUPPP_SCRIPT_API_H
#define LUPPP_SCRIPT_API_H

/**
 * This file is the API for the Controller Scripts, allowing them to
 * understand the events that have occurred in Luppp, and should be shown
 * on the hardware device. Similarly, this API provides functions to send
 * events to Luppp, which allow controlling of Luppp from the hardware.
 */

enum EVENT_ID {
	EVENT_NOP = 0,
	EVENT_TRACK_RECORD_ARM,
	EVENT_TRACK_VOLUME,
	EVENT_TRACK_SEND,
	EVENT_TRACK_SEND_ACTIVE,
	EVENT_TRACK_JACKSEND,
	EVENT_TRACK_JACKSEND_ACTIVE,
};

struct event_track_record_arm {
	int track;
	int armed;
};
struct event_track_jack_send {
	int track;
	float value;
};
struct event_track_jack_send_active {
	int track;
	int active;
};
struct event_track_volume {
	int track;
	float value;
};
struct event_track_send {
	int track;
	int send;
	float value;
};
struct event_track_send_active {
	int track;
	int send;
	int active;
};

/*
struct event {
	enum EVENT_ID event_id;
	union {
		struct event_track_send track_send;
		struct event_track_send_active track_send_active;
	};
};
*/

void luppp_do(enum EVENT_ID id, void* event_struct);
void luppp_write_midi(void *ctlr, unsigned char* midi);

#endif /* LUPPP_SCRIPT_API_H */

