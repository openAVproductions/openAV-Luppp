#include "luppp_script_api.h"

int test_poll(unsigned char* midi)
{
	if(midi[1] == 0x42) {
		printf("play pressed\n");
	}
	if(midi[1] == 0x3c) {
		printf("cue pressed\n");
	}
	if(midi[1] == 0x3b) {
		printf("demo pressed\n");
	}
	if(midi[1] == 0x33) {
		printf("33 pressed\n");
	}
	if(midi[1] == 0x47) {
		printf("47 pressed\n");
	}
	if(midi[1] == 0x66) {
		printf("66 pressed - sending track active\n");
		struct event_track_send_active e = {
			.track = 3,
			.send = 0,
			.active = midi[0] == 0x90,
		};
		luppp_do(EVENT_TRACK_SEND_ACTIVE, &e);
	}

	static int lookup[0x17];
	lookup[0x8 ] = 0;
	lookup[0x17] = 1;
	lookup[0xb ] = 2;
	lookup[0x9 ] = 3;

	if(midi[0] == 0xb0) {
		if(midi[1] == 0x17 ||
		   midi[1] == 0x8  ||
		   midi[1] == 0xb  ||
		   midi[1] == 0x9) {
			struct event_track_send e = {
				.track = lookup[midi[1]],
				.send = 0,
				.value = midi[2] / 127.f,
			};
			luppp_do(EVENT_TRACK_SEND, &e);
		}

		if(midi[1] == 0xd || midi[1] == 0xe) {
			struct event_track_volume e = {
				.track = (midi[1] == 0xe),
				.value = midi[2] / 127.f,
			};
			luppp_do(EVENT_TRACK_VOLUME, &e);
		}
	}
	
	return 0;
}

void test_handle(void *ctlr, enum EVENT_ID id, void *event)
{
	printf("test_handle event id %d\n", id);
	switch(id) {
	case EVENT_TRACK_SEND_ACTIVE: {
		struct event_track_send_active *e =
			(struct event_track_send_active *)event;
		printf("Script t %d, s %d, active %d\n", e->track, e->send, e->active);
		unsigned char led_on [3] = {0x90, 0x42, 0x7F};
		unsigned char led_off[3] = {0x80, 0x42, 0};
		if(e->active)
			luppp_write_midi(ctlr, led_on);
		else
			luppp_write_midi(ctlr, led_off);
		} break;
	};
}
