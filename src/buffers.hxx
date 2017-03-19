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

#ifndef LUPPP_BUFFERS_H
#define LUPPP_BUFFERS_H

#include <cstring>
#include <jack/transport.h>

class Buffers
{
public:

	enum BUFFER {
		// AUDIO
		MASTER_INPUT_L = 0,
		MASTER_INPUT_R,

		MASTER_OUT_L,
		MASTER_OUT_R,

		JACK_SEND_OUT_L,
		JACK_SEND_OUT_R,
		JACK_MASTER_OUT_L,
		JACK_MASTER_OUT_R,
		JACK_SIDECHAIN_KEY_L,
		JACK_SIDECHAIN_KEY_R,
		JACK_SIDECHAIN_SIGNAL_L,
		JACK_SIDECHAIN_SIGNAL_R,

		SEND_L,
		SEND_R,
		SIDECHAIN_KEY_L,
		SIDECHAIN_KEY_R,
		SIDECHAIN_SIGNAL_L,
		SIDECHAIN_SIGNAL_R,

		MASTER_RETURN_L,
		MASTER_RETURN_R,

		HEADPHONES_OUT_L,
		HEADPHONES_OUT_R,

		// MIDI
		MASTER_MIDI_INPUT,

		// track buffers: they are the "working" buffers per track:
		// the end result is mixed into the master output, while each
		// stage along the way the amplitude etc can be analysed
		TRACK_0_L,
		TRACK_0_R,
		TRACK_1_L,
		TRACK_1_R,
		TRACK_2_L,
		TRACK_2_R,
		TRACK_3_L,
		TRACK_3_R,
		TRACK_4_L,
		TRACK_4_R,
		TRACK_5_L,
		TRACK_5_R,
		TRACK_6_L,
		TRACK_6_R,
		TRACK_7_L,
		TRACK_7_R,
		//Per track sends/returns
		SEND_TRACK_0_L,
		SEND_TRACK_0_R,
		SEND_TRACK_1_L,
		SEND_TRACK_1_R,
		SEND_TRACK_2_L,
		SEND_TRACK_2_R,
		SEND_TRACK_3_L,
		SEND_TRACK_3_R,
		SEND_TRACK_4_L,
		SEND_TRACK_4_R,
		SEND_TRACK_5_L,
		SEND_TRACK_5_R,
		SEND_TRACK_6_L,
		SEND_TRACK_6_R,
		SEND_TRACK_7_L,
		SEND_TRACK_7_R,
		RETURN_TRACK_0_L,
		RETURN_TRACK_0_R,
		RETURN_TRACK_1_L,
		RETURN_TRACK_1_R,
		RETURN_TRACK_2_L,
		RETURN_TRACK_2_R,
		RETURN_TRACK_3_L,
		RETURN_TRACK_3_R,
		RETURN_TRACK_4_L,
		RETURN_TRACK_4_R,
		RETURN_TRACK_5_L,
		RETURN_TRACK_5_R,
		RETURN_TRACK_6_L,
		RETURN_TRACK_6_R,
		RETURN_TRACK_7_L,
		RETURN_TRACK_7_R,

		JACK_TRACK_0_L,
		JACK_TRACK_0_R,
		JACK_TRACK_1_L,
		JACK_TRACK_1_R,
		JACK_TRACK_2_L,
		JACK_TRACK_2_R,
		JACK_TRACK_3_L,
		JACK_TRACK_3_R,
		JACK_TRACK_4_L,
		JACK_TRACK_4_R,
		JACK_TRACK_5_L,
		JACK_TRACK_5_R,
		JACK_TRACK_6_L,
		JACK_TRACK_6_R,
		JACK_TRACK_7_L,
		JACK_TRACK_7_R,


		BUFFER_COUNT,
	};

	Buffers()
	{
		memset( audio, 0, sizeof(float*)*BUFFER_COUNT);
		memset( midi , 0, sizeof(void *)*BUFFER_COUNT);
	}
	float* audio[BUFFER_COUNT];
	void*  midi [BUFFER_COUNT];

	// Jack details
	jack_nframes_t         nframes;
	jack_nframes_t         samplerate;

	jack_nframes_t          transportFrame;
	jack_position_t*        transportPosition;
	jack_transport_state_t* transportState;

};

#endif // LUPPP_BUFFERS_H

