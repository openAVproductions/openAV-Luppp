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

#include "ctlra.hxx"

#include <errno.h>
#include <iostream>
#include <sys/stat.h>

#include "../jack.hxx"
#include "../event.hxx"
#include "../logic.hxx"
#include "../gridlogic.hxx"
#include "../timemanager.hxx"

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

void luppp_write_ctlra_light(void *ctlra, int light, int light_status)
{
	((CtlraScript*)ctlra)->writeCtlraLight(light, light_status);
}

void CtlraScript::writeCtlraLight(int light, int status)
{
	struct ctlra_dev_t *hack_dev = (struct ctlra_dev_t *)hack_current_dev;
	ctlra_dev_light_set(hack_dev, light, status);
	//ctlra_dev_light_flush(hack_dev, 0);
}

void luppp_do(enum EVENT_ID id, void* e)
{
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
	case EVENT_TRACK_VOLUME: {
		struct event_track_volume *ev = (struct event_track_volume *)e;
		jack->getLogic()->trackVolume(ev->track, ev->value);
		break;
		}
	case EVENT_TRACK_RECORD_ARM: {
		struct event_track_record_arm *ev =
			(struct event_track_record_arm *)e;
		jack->getLogic()->trackRecordArm(ev->track, ev->armed);
		break;
		}
	case EVENT_TRACK_JACKSEND: {
		struct event_track_jack_send *ev =
			(struct event_track_jack_send *)e;
		jack->getLogic()->trackJackSend(ev->track, ev->value);
		break;
		}
	case EVENT_TRACK_JACKSEND_ACTIVE: {
		struct event_track_jack_send_active *ev =
			(struct event_track_jack_send_active *)e;
		jack->getLogic()->trackJackSendActivate(ev->track, ev->active);
		break;
		}
	case EVENT_GRID_PRESS_RELEASE: {
		struct event_grid_press_release *ev =
			(struct event_grid_press_release *)e;
		/* TODO: Refactor to GridLogic::press_release(int pressed);*/
		if(ev->pressed)
			jack->getGridLogic()->pressed(ev->track, ev->scene);
		else
			jack->getGridLogic()->released(ev->track, ev->scene);
		break;
		}
	case EVENT_GRID_LAUNCH_SCENE: {
		struct event_grid_launch_scene *ev =
			(struct event_grid_launch_scene *)e;
		jack->getGridLogic()->launchScene(ev->scene);
		break;
		}
	case EVENT_TEMPO_BPM: {
		struct event_tempo_bpm *ev = (struct event_tempo_bpm *)e;
		jack->getTimeManager()->setBpm(ev->bpm);
		break;
		}
	case EVENT_TEMPO_TAP: {
		struct event_tempo_tap *ev = (struct event_tempo_tap *)e;
		/* No params, this counts *when* the event arrives, not
		 * what it contains. A boolean integer indicates pressed
		 * or released state of the button for LEDs */
		jack->getTimeManager()->tap();
		break;
		}
	case EVENT_MASTER_VOLUME: {
		struct event_master_volume *ev = (struct event_master_volume *)e;
		jack->masterVolume(ev->volume);
		break;
		}
	case EVENT_MASTER_RETURN: {
		struct event_master_return*ev = (struct event_master_return *)e;
		jack->returnVolume(ev->volume);
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

const char *CtlraScript::getCtlraName(int ctlr_type, int ctlr_id)
{
	struct ctlra_dev_t * d = (struct ctlra_dev_t *)hack_current_dev;
	struct ctlra_dev_info_t dev_info;
	ctlra_dev_get_info(d, &dev_info);

	const int c = 5 ;//dev_info.control_count[CTLRA_EVENT_SLIDER];
	/*
	for(int i = 0; i < c; i++) {
		const char *n = ctlra_info_get_name(&dev_info,
						    (enum ctlra_event_type_t)ctlr_type, ctlr_id);
		printf("%s\n", n);
	}
	*/
	return 0;
}

int CtlraScript::compile()
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

	tcc_add_symbol(s, "luppp_write_ctlra_light",
		       (void *)luppp_write_ctlra_light);
	if(ret < 0) {
		error("failed to insert luppp_write_ctlra_light() symbol\n");
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

	tcc_event_func = (tcc_event_func_t)tcc_get_symbol(s, "test_ctlra");
	if(!tcc_event_func) {
		error("failed to get tcc_event_func\n");
	}

	/* handles events from Luppp */
	handle = (ctlra_handle_event)tcc_get_symbol(s, "test_handle");
	if(!handle)
		error("failed to get test handle\n");

	tcc_delete(s);

	/* Store the time of compiling */
	file_modify_time(filename.c_str(), &script_load_time);
	program_ok = 1;
	printf("compiled ok\n");

	//getCtlraName(0, 0);

	return 0;
}

void CtlraScript::script_reload()
{
	time_t new_time;
	int err = file_modify_time(filename.c_str(), &new_time);
	if(err)
		return;

	if(new_time > script_load_time) {
		printf("CtlraScript %d : Reloading script \"%s\"\n",
		       getID(), filename.c_str());
		compile();
	}
}


/* Non static version of the same - needed to access the tcc functions */
void CtlraScript::ctlra_forward(struct ctlra_dev_t* dev, uint32_t num_events,
		   struct ctlra_event_t** events)
{
	hack_current_dev = dev;
	script_reload();
	printf("here\n");
	if(tcc_event_func) {
		tcc_event_func(num_events, events);
	}
	//ctlra_dev_light_flush((struct ctlra_dev_t *)hack_current_dev, 0);
}

static void
luppp_event_func(struct ctlra_dev_t* dev, uint32_t num_events,
		 struct ctlra_event_t** events, void *userdata)
{
	printf("luppp event func\n");
	CtlraScript* c = (CtlraScript*)userdata;
	c->ctlra_forward(dev, num_events, events);
}

void CtlraScript::poll_devices()
{
	if(!dev) {
		printf("%s error, no dev!\n", __func__);
		return;
	}

	//ctlra_rt_safe_idle_iter(dev);
	ctlra_idle_iter(dev);
}

static int
accept_dev_func(struct ctlra_t *ctlra,
		const struct ctlra_dev_info_t *info,
		struct ctlra_dev_t *dev,
		void *userdata)
{
	printf("LupppCtlra: accepting %s %s\n", info->vendor, info->device);
	ctlra_dev_set_event_func(dev, luppp_event_func);
	/*
	ctlra_dev_set_feedback_func(dev, luppp_feedback_func);
	ctlra_dev_set_screen_feedback_func(dev, luppp_screen_redraw_func);
	ctlra_dev_set_remove_func(dev, luppp_remove_func);
	*/
	ctlra_dev_set_callback_userdata(dev, userdata);
	return 1;
}

CtlraScript::CtlraScript(std::string file) :
	Controller(),
	MidiIO(),
	program(0)
{
	/* Step 1: compile script */
	this->filename = file;
	LUPPP_NOTE("CtlraScript() got file %s", file.c_str());

	int err = compile();
	if(err) {
		printf("compile failed\n");
		return;
	}

	struct ctlra_create_opts_t opts = {0};
	//opts.flags_rt_safe = 0;
	dev = ctlra_create(&opts);
	if(!dev) {
		printf("Warning, failed to create ctlra instance!!\n");
		return;
	}

	// TODO: rework accept dev to new signature
	int num_devs = ctlra_probe(dev, accept_dev_func, this);
	LUPPP_NOTE("Num devs %d\n", num_devs);

	stat = CONTROLLER_OK;
}

CtlraScript::~CtlraScript()
{
	if(program)
		free(program);
	ctlra_exit(dev);
}

int CtlraScript::registerComponents()
{
	// makes JACK add this controller to the midiObservers list:
	// note the static_cast<>() is *needed* here for multiple-inheritance
	MidiIO* m = static_cast<MidiIO*>(this);

	printf("registering ports!!\n");
	registerMidiPorts( "testScripted" );

	jack->registerMidiIO( m );

	return LUPPP_RETURN_OK;
}


Controller::STATUS CtlraScript::status()
{
	return stat;
}

void CtlraScript::midi(unsigned char* midi)
{
	script_reload();

	if(!program_ok)
		return;

	int status  = midi[0];
	int data    = midi[1];
	float value = midi[2] / 127.f;

	//printf("%s : got %2x %2x %0.2f\n", __func__, status, data, value);
	//int ret = poll(midi);
}

struct event_trac_send_active_t {
	int type;
	int track;
	int send;
	int active;
};

void CtlraScript::trackSendActive(int t, int send, bool a)
{
	if(!program_ok)
		return;

	struct event_track_send_active ev = {
		.track = t,
		.send = send,
		.active = 0,
	};
	if(a)
		ev.active = 1;
	handle(this, EVENT_TRACK_SEND_ACTIVE, &ev);
	ctlra_dev_light_flush((struct ctlra_dev_t *)hack_current_dev, 0);
}

void CtlraScript::barBeat(int bar, int beat)
{
	if(!program_ok)
		return;

	struct event_time_bar_beat ev = {
		.bar = bar,
		.beat = beat,
	};
	handle(this, EVENT_TIME_BAR_BEAT, &ev);
	ctlra_dev_light_flush((struct ctlra_dev_t *)hack_current_dev, 0);
}

void CtlraScript::setSceneState(int track, int scene, GridLogic::State s)
{
	struct event_grid_press_release ev = {
		.track = track,
		.scene = scene,
		.pressed = (int)s,
	};
	handle(this, EVENT_GRID_PRESS_RELEASE, &ev);
	ctlra_dev_light_flush((struct ctlra_dev_t *)hack_current_dev, 0);
}

