#include "luppp_script_api.h"

#include "pstdint.h"

#include "event.h"


#define MODE_SCENE 0
#define MODE_PATTERN 1
#define MODE_PAD 2
#define MODE_VOLUME 3

static int mode;
static int shift;
static float volumes[4];

int test_ctlra(int num, struct ctlra_event_t **events)
{
	//printf("test_ctlra\n");
	for(uint32_t i = 0; i < num; i++) {
		const char *pressed = 0;
		struct ctlra_event_t *e = events[i];
		const char *name = 0;
		switch(e->type) {
		case CTLRA_EVENT_BUTTON: {
			int id = e->button.id;
			printf("button id = %d\n", id);
			switch(id) {
			case 7: shift = e->button.pressed; break;
			case 21: mode = MODE_SCENE; break;
			case 22: mode = MODE_PATTERN; break;
			case 23: mode = MODE_PAD; break;
			case 24: if(shift) mode = MODE_VOLUME; break;

			case 63: printf("ecndoer press\n");
				struct event_grid_launch_scene ev = {
					4
				};
				luppp_do(EVENT_GRID_LAUNCH_SCENE, &ev);
				break;

			default: printf("button %d\n", id); break;
			}
			break;
		}

		case CTLRA_EVENT_GRID: {
			int pos = e->grid.pos;
			//printf("grid id = %d, pos %d, pressed %d\n", e->grid.id, e->grid.pos, e->grid.pressed);
			if(mode == MODE_VOLUME) {
				static float vols[] = {1, 0.5, 0.3, 0.15};
				int t = e->grid.pos % 4;
				float v = vols[e->grid.pos / 4];
				struct event_track_volume ev = {t, v};
				volumes[t] = v;
				luppp_do(EVENT_TRACK_VOLUME, &ev);
			}
			if(mode == MODE_SCENE) {
				struct event_grid_launch_scene ev =
					{e->grid.pos / 4};
				luppp_do(EVENT_GRID_LAUNCH_SCENE, &ev);
			}
			if(mode == MODE_PATTERN) {
				int t = e->grid.pos % 4;
				int s = e->grid.pos / 4;
				struct event_grid_press_release ev =
					{t, s, e->grid.pressed};
				luppp_do(EVENT_GRID_PRESS_RELEASE, &ev);
			}
			break;
		}

		case CTLRA_EVENT_SLIDER: {
			int id = e->slider.id;
			//printf("slider %d\n", id);
			if(id < 4) {
				struct event_track_volume ev = {
					.track = id,
					.value = e->slider.value,
				};
				luppp_do(EVENT_TRACK_VOLUME, &ev);
			} else {
				struct event_track_send ev = {
					.track = id - 4,
					.send = 0,
					.value = e->slider.value,
				};
				luppp_do(EVENT_TRACK_SEND, &ev);
			}
			break;
		}
		} /* switch */
	}
}

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
	switch(id) {
	case EVENT_TRACK_SEND_ACTIVE: {
		struct event_track_send_active *e =
			(struct event_track_send_active *)event;
		//printf("Script t %d, s %d, active %d\n", e->track, e->send, e->active);
		unsigned char led_on [3] = {0x90, 0x42, 0x7F};
		unsigned char led_off[3] = {0x80, 0x42, 0};
#if 0
		if(e->active)
			//luppp_write_midi(ctlr, led_on);
		else
			//luppp_write_midi(ctlr, led_off);
#endif
		} break;
	case EVENT_GRID_PRESS_RELEASE: {
		struct event_grid_press_release *e =
			(struct event_grid_press_release *)event;
		//printf("pressed = %d\n", e->pressed);
		uint32_t col[] = {
			0xff000000,
			0x0000ff00,
			0x000041ff,
			0x00ff5100,
			0x000041ff,
			0x00ff0000,
			0x0000ffff,
		};
		if(e->track > 3 || e->scene > 3)
			break;
		int light = (e->scene * 4) + e->track + 28;
		//printf("light = %d\n", light);
		luppp_write_ctlra_light(ctlr, light, col[e->pressed]);
		} break;
	case EVENT_TIME_BAR_BEAT: {
		struct event_time_bar_beat *e =
			(struct event_time_bar_beat *)event;
		//printf("Script bar %d, beat %d\n", e->bar, e->beat);
		int on = e->beat % 4;
		int off = (e->beat+3) % 4;
		uint32_t ids[] = {16, 13, 14, 19};
		luppp_write_ctlra_light(ctlr, ids[on], UINT32_MAX);
		int i;
		for(i = on; i < 3; i++)
			luppp_write_ctlra_light(ctlr, ids[off-i], 0x4000000);
		} break;
	};

	/* TODO: port this to the feedback API: handle "other" stuff */
	luppp_write_ctlra_light(ctlr, 20, (mode == MODE_SCENE) * UINT32_MAX);
	luppp_write_ctlra_light(ctlr, 21, (mode == MODE_PATTERN) * UINT32_MAX);
	luppp_write_ctlra_light(ctlr, 22, (mode == MODE_PAD) * UINT32_MAX);

	luppp_write_ctlra_light(ctlr, 23, (mode == MODE_VOLUME) * UINT32_MAX);
	if(mode == MODE_VOLUME) {
		const float levels[] = {1, 0.5, 0.3, 0.15};
		for(int j = 0; j < 4; j++)
			for(int i = 0; i < 4; i++)
				luppp_write_ctlra_light(ctlr, 28 + j + i * 4,
					(volumes[j] >= levels[i]) * UINT32_MAX);
	}
}
