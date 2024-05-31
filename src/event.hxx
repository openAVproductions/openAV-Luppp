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

#ifndef LUPPP_EVENT_H
#define LUPPP_EVENT_H

#include <iostream>
#include <stdint.h>

/*
    event.hxx

  This file provides declarations for each type of event that the engine uses.
*/

#include "looper.hxx"
#include "gridlogic.hxx"
#include "transport.hxx"

#pragma GCC diagnostic ignored "-Wunused-parameter"

using namespace std;

namespace Event
{
enum SEND_TYPE {
	SEND_POSTFADER = 0,
	SEND_KEY,
	SEND_XSIDE,
};
enum RETURN_TYPE {
	RETURN_INVALID = 0,
	RETURN_MAIN,
};
enum INPUT_TO {
	INPUT_TO_SEND = 0,
	INPUT_TO_MIX,
	INPUT_TO_XSIDE,
	INPUT_TO_SIDE_KEY,
};
enum SOURCE {
	SOURCE_UNKNOWN = 0,
	SOURCE_MIDI,
	SOURCE_GUI,
	SOURCE_LOAD,
};

enum EVENT_TYPE {
	// default event type
	EVENT_NULL = 0,

	MASTER_VOL,
	MASTER_INPUT_VOL,
	MASTER_INPUT_TO,
	MASTER_INPUT_TO_ACTIVE,
	MASTER_RETURN,
	RECORD,

	SESSION_SAVE,       // save hole session
	CLIP_SAVE,	 // save single clip
	STATE_RESET, // reset all state
	STATE_SAVE_FINISH,// save action finished, flush metadata to disk
	STATE_SAVE_BUFFER,// save an individual AudioBuffer* to disk

	REQUEST_SAVE_BUFFER, // gets an audioBuffer of a certain size

	/// Grid
	GRID_EVENT, // press / release events
	GRID_STATE, // state of one block
	GRID_LAUNCH_SCENE, // launches a scene
	GRID_SELECT_CLIP_ENABLE, // enable selecting a clip from the grid
	GRID_SELECT_CLIP_EVENT, // a press / release on the selected clip
	GRID_SELECT_NEW_CHOSEN, // a different clip is now "special"

	/// Track
	TRACK_JACKSEND,
	TRACK_JACKSEND_ACTIVATE,
	TRACK_SEND,
	TRACK_SEND_ACTIVE,
	TRACK_SIGNAL_LEVEL,
	TRACK_VOLUME,
	TRACK_PAN,
	TRACK_RECORD_ARM,

	FX_REVERB,

	LOOPER_LOAD,
	LOOPER_STATE,
	LOOPER_PROGRESS,
	LOOPER_LOOP_LENGTH,
	LOOPER_LOOP_USE_AS_TEMPO,

	/// Transport etc
	METRONOME_ACTIVE,
	METRONOME_VOLUME,
	TRANSPORT,                /// rolling or stopped

	TIME_BPM,
	TIME_BAR_BEAT,
	TIME_TEMPO_TAP,

	GUI_PRINT,

	LOOPER_REQUEST_BUFFER,
	DEALLOCATE_BUFFER,

	SAMPLERATE,

	// Controller
	CONTROLLER_INSTANCE,
	CONTROLLER_INSTANCE_REMOVE,
	CONTROLLER_INSTANCE_GET_TO_WRITE,
	CONTROLLER_BINDING_ENABLE,
	CONTROLLER_BINDING_TARGET,
	CONTROLLER_BINDING_MADE,
	CONTROLLER_BINDING_REMOVE,

	QUIT,

	// for keeping loop index's inside the enum
	EVENT_TYPE_FINAL,
};

/// returns the pretty name of an event
const char* getPrettyName( int type );

/// returns an EVENT_TYPE from a pretty name
EVENT_TYPE getTypeFromName(const char* name);
};

using namespace Event;

class AudioBuffer;

class EventBase
{
public:
	virtual ~EventBase() {}

	virtual int type() = 0;
	virtual uint32_t size() = 0;

	/// returns const char* to static char buffer: is the pretty name for event
	/// optional override: if function isn't overriden, no target update sent
	virtual const char* name()
	{
		return 0;
	}
};

class EventTransportState : public EventBase
{
public:
	int type()
	{
		return int(TRANSPORT);
	}
	uint32_t size()
	{
		return sizeof(EventTransportState);
	}
	TRANSPORT_STATE ts;
	EventTransportState(): ts( TRANSPORT_STOPPED ) {}
	EventTransportState( TRANSPORT_STATE t): ts(t) {}
};

class EventControllerBindingMade : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_BINDING_MADE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerBindingMade);
	}
	int controllerID;
	void* binding;
	EventControllerBindingMade(int id = 0, void* b=0): controllerID(id), binding(b) {}
};

class EventControllerBindingRemove : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_BINDING_REMOVE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerBindingRemove);
	}
	int controllerID;
	int bindingID;
	void* binding;
	EventControllerBindingRemove(int ctlrID = 0, int bindID=0, void* b=0): controllerID(ctlrID), bindingID(bindID), binding(b) {}
};

class EventControllerBindingEnable : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_BINDING_ENABLE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerBindingEnable);
	}
	int controllerID;
	bool enable;
	EventControllerBindingEnable(int id = 0, bool e=false):controllerID(id),enable(e) {}
};

class EventGridSelectClipEvent : public EventBase
{
public:
	int type()
	{
		return int(GRID_SELECT_CLIP_EVENT);
	}
	uint32_t size()
	{
		return sizeof(EventGridSelectClipEvent);
	}
	bool pressed;
	EventGridSelectClipEvent(bool p=false):pressed(p) {}
};

class EventGridSelectClipEnable : public EventBase
{
public:
	int type()
	{
		return int(GRID_SELECT_CLIP_ENABLE);
	}
	uint32_t size()
	{
		return sizeof(EventGridSelectClipEnable);
	}
	bool enable;
	EventGridSelectClipEnable(bool e=false):enable(e) {}
};

class EventGridSelectNewChosen : public EventBase
{
public:
	int type()
	{
		return int(GRID_SELECT_NEW_CHOSEN);
	}
	uint32_t size()
	{
		return sizeof(EventGridSelectNewChosen);
	}
	int track;
	int scene;
	EventGridSelectNewChosen(int t = -1, int s = -1):track(t),scene(s) {}
};

class EventQuit : public EventBase
{
public:
	int type()
	{
		return int(QUIT);
	}
	uint32_t size()
	{
		return sizeof(EventQuit);
	}
	EventQuit() {}
};

class EventSamplerate : public EventBase
{
public:
	int type()
	{
		return int(SAMPLERATE);
	}
	uint32_t size()
	{
		return sizeof(EventSamplerate);
	}
	int samplerate;
	EventSamplerate(int sr = 0): samplerate(sr) {}
};

class EventControllerInstance : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_INSTANCE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerInstance);
	}
	void* controller;
	EventControllerInstance(void* c = 0) : controller(c) {}
};

class EventControllerInstanceRemove : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_INSTANCE_REMOVE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerInstanceRemove);
	}
	int ID;
	EventControllerInstanceRemove(int i = -1) : ID(i) {}
};

/// writes the GenericMIDI controller instance to a .ctlr file
class EventControllerInstanceGetToWrite : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_INSTANCE_GET_TO_WRITE);
	}
	uint32_t size()
	{
		return sizeof(EventControllerInstanceGetToWrite);
	}
	int ID;
	void* controller;
	EventControllerInstanceGetToWrite(int id = 0, void* c = 0) : ID(id), controller(c) {}
};

class EventMasterInputTo : public EventBase
{
public:
	int type()
	{
		return int(MASTER_INPUT_TO);
	}
	uint32_t size()
	{
		return sizeof(EventMasterInputTo);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int place;
	float value;
	EventMasterInputTo(int p=-1, float v=0) : place(p), value(v) {}
};

class EventMasterInputToActive : public EventBase
{
public:
	int type()
	{
		return int(MASTER_INPUT_TO_ACTIVE);
	}
	uint32_t size()
	{
		return sizeof(EventMasterInputToActive);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int place;
	bool active;
	EventMasterInputToActive(int p=-1, bool a=false) : place(p), active(a) {}
};

class EventMasterVol : public EventBase
{
public:
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}
	int type()
	{
		return int(MASTER_VOL);
	}
	uint32_t size()
	{
		return sizeof(EventMasterVol);
	}

	float vol;
	SOURCE source;
	EventMasterVol(float v = 0, SOURCE s = SOURCE_UNKNOWN) : vol(v), source(s) {}
};

class EventMasterInputVol : public EventBase
{
public:
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}
	int type()
	{
		return int(MASTER_INPUT_VOL);
	}
	uint32_t size()
	{
		return sizeof(EventMasterInputVol);
	}
	float vol;
	EventMasterInputVol(float v = 0) : vol(v) {}
};

class EventMasterReturn : public EventBase
{
public:
	int type()
	{
		return int(MASTER_RETURN);
	}
	uint32_t size()
	{
		return sizeof(EventMasterReturn);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	RETURN_TYPE ret;
	float vol;

	EventMasterReturn(RETURN_TYPE st=RETURN_INVALID, float v=0): ret(st), vol(v) {}
};

class EventTrackVol : public EventBase
{
public:
	int type()
	{
		return int(TRACK_VOLUME);
	}
	uint32_t size()
	{
		return sizeof(EventTrackVol);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	float vol;
	SOURCE source;

	EventTrackVol() {};
	EventTrackVol(int t, float v, SOURCE s): track(t), vol(v), source(s) { }
};

class EventTrackPan : public EventBase
{
public:
	int type()
	{
		return int(TRACK_PAN);
	}
	uint32_t size()
	{
		return sizeof(EventTrackPan);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	float pan;
	SOURCE source;

	EventTrackPan() {};
	EventTrackPan(int t, float p, SOURCE s): track(t), pan(p), source(s) { }
};


class EventTrackRecordArm : public EventBase
{
public:
	int type()
	{
		return int(TRACK_RECORD_ARM);
	}
	uint32_t size()
	{
		return sizeof(EventTrackRecordArm);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	float recordArm;

	EventTrackRecordArm() {};
	EventTrackRecordArm(int t, bool r)
	{
		track = t;
		recordArm = r;
	}
};

class EventSessionSave : public EventBase
{
public:
	int type()
	{
		return int(SESSION_SAVE);
	}
	uint32_t size()
	{
		return sizeof(EventSessionSave);
	}

	EventSessionSave() {}
};

class EventClipSave : public EventBase
{
public:
	int type()
	{
		return int(CLIP_SAVE);
	}
	uint32_t size()
	{
		return sizeof(EventClipSave);
	}

	int track;
	int scene;

	EventClipSave(): track(0), scene(0) {};
	EventClipSave(int t, int s): track(t), scene(s) {}
};

class EventStateReset : public EventBase
{
public:
	int type()
	{
		return int(STATE_RESET);
	}
	uint32_t size()
	{
		return sizeof(EventStateReset);
	}

	EventStateReset() {}
};

class EventStateSaveFinish : public EventBase
{
public:
	int type()
	{
		return int(STATE_SAVE_FINISH);
	}
	uint32_t size()
	{
		return sizeof(EventStateSaveFinish);
	}

	EventStateSaveFinish() {};
};

class EventGridEvent : public EventBase
{
public:
	int type()
	{
		return int(GRID_EVENT);
	}
	uint32_t size()
	{
		return sizeof(EventGridEvent);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	int scene;
	bool pressed;

	EventGridEvent() {};
	EventGridEvent(int t, int s, bool p): track(t), scene(s), pressed(p) {}
};

class EventGridState : public EventBase
{
public:
	int type()
	{
		return int(GRID_STATE);
	}
	uint32_t size()
	{
		return sizeof(EventGridState);
	}

	int track;
	int scene;
	GridLogic::State state;

	EventGridState() {};
	EventGridState(int t, int s, GridLogic::State st): track(t), scene(s), state(st) {}
};

class EventGridLaunchScene : public EventBase
{
public:
	int type()
	{
		return int(GRID_LAUNCH_SCENE);
	}
	uint32_t size()
	{
		return sizeof(EventGridLaunchScene);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int scene;

	EventGridLaunchScene() {};
	EventGridLaunchScene(int s): scene(s) {}
};

class EventFxReverb : public EventBase
{
public:
	int type()
	{
		return int(FX_REVERB);
	}
	uint32_t size()
	{
		return sizeof(EventFxReverb);
	}

	bool enable;

	float rtSize;
	float wet;
	float damping;

	EventFxReverb() {};
	EventFxReverb(bool e, float s, float w, float d): enable(e), rtSize(s), wet(w), damping(d) {}
};

class EventTrackSend : public EventBase
{
public:
	int type()
	{
		return int(TRACK_SEND);
	}
	uint32_t size()
	{
		return sizeof(EventTrackSend);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	SEND_TYPE send;
	float value;
	SOURCE source;

	EventTrackSend() {};
	EventTrackSend(int t, SEND_TYPE st, float v, SOURCE s): track(t), send(st), value(v), source(s) {}
};

class EventTrackJackSend : public EventBase
{
public:
	int type()
	{
		return int(TRACK_JACKSEND);
	}
	uint32_t size()
	{
		return sizeof(EventTrackJackSend);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	float value;
	SOURCE source;

	EventTrackJackSend() {};
	EventTrackJackSend(int t, float v, SOURCE s): track(t), value(v), source(s) {}
};

class EventTrackSendActive : public EventBase
{
public:
	int type()
	{
		return int(TRACK_SEND_ACTIVE);
	}
	uint32_t size()
	{
		return sizeof(EventTrackSendActive);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	SEND_TYPE send;
	bool active;

	EventTrackSendActive() {};
	EventTrackSendActive(int t, SEND_TYPE s, bool a): track(t), send(s), active(a) {}
};

class EventTrackJackSendActivate : public EventBase
{
public:
	int type()
	{
		return int(TRACK_JACKSEND_ACTIVATE);
	}
	uint32_t size()
	{
		return sizeof(EventTrackJackSendActivate);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	int track;
	bool active;

	EventTrackJackSendActivate() {};
	EventTrackJackSendActivate(int t, bool a): track(t), active(a) {}
};
class EventLooperState : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_STATE);
	}
	uint32_t size()
	{
		return sizeof(EventLooperState);
	}

	int track;
	int scene;
	//Looper::State state;

	EventLooperState() {}
	//EventLooperState(int t, int sc, Looper::State s) : track(t), scene(sc), state(s){}
};

class EventLooperProgress : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_PROGRESS);
	}
	uint32_t size()
	{
		return sizeof(EventLooperProgress);
	}

	int track;
	float progress;
	EventLooperProgress() {}
	EventLooperProgress(int t, float p) : track(t), progress(p) {}
};

class EventLooperLoopLength : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_LOOP_LENGTH);
	}
	uint32_t size()
	{
		return sizeof(EventLooperLoopLength);
	}

	int track;
	int scene;
	int beats;
	EventLooperLoopLength() {}
	EventLooperLoopLength(int t, int s, int b) : track(t), scene(s), beats(b) {}
};

class EventLooperUseAsTempo : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_LOOP_USE_AS_TEMPO);
	}
	uint32_t size()
	{
		return sizeof(EventLooperUseAsTempo);
	}

	int track;
	int scene;
	int beats;
	EventLooperUseAsTempo() {}
	EventLooperUseAsTempo(int t, int s) : track(t), scene(s) {}
};

class EventLooperLoad : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_LOAD);
	}
	uint32_t size()
	{
		return sizeof(EventLooperLoad);
	}

	int track;
	int clip;
	void* audioBuffer;

	EventLooperLoad() {}
	EventLooperLoad(int t, int c, void* ab) : track(t), clip(c), audioBuffer(ab)
	{
	}
};

class EventMetronomeActive : public EventBase
{
public:
	int type()
	{
		return int(METRONOME_ACTIVE);
	}
	uint32_t size()
	{
		return sizeof(EventMetronomeActive);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	bool active;
	EventMetronomeActive(bool a = false) : active(a) {}
};

class EventMetronomeVolume : public EventBase
{
public:
	int type()
	{
		return int(METRONOME_VOLUME);
	}
	uint32_t size()
	{
		return sizeof(EventMetronomeVolume);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	float vol;
	SOURCE source;

	EventMetronomeVolume(float v = 0.f, SOURCE s = SOURCE_UNKNOWN) : vol(v), source(s) {}
};

class EventTimeBPM : public EventBase
{
public:
	int type()
	{
		return int(TIME_BPM);
	}
	uint32_t size()
	{
		return sizeof(EventTimeBPM);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	float bpm;

	EventTimeBPM() {}
	EventTimeBPM(float b) : bpm(b) {}
};

class EventTrackSignalLevel : public EventBase
{
public:
	int type()
	{
		return int(TRACK_SIGNAL_LEVEL);
	}
	uint32_t size()
	{
		return sizeof(EventTrackSignalLevel);
	}

	int track;
	float left;
	float right;

	EventTrackSignalLevel() {}
	EventTrackSignalLevel(int t, float l,float r) : track(t), left(l), right(r) {}
};

class EventTimeTempoTap : public EventBase
{
public:
	int type()
	{
		return int(TIME_TEMPO_TAP);
	}
	uint32_t size()
	{
		return sizeof(EventTimeTempoTap);
	}
	static const char* prettyName;
	const char* name()
	{
		return prettyName;
	}

	bool pressed; // only used to turn on / off the button in GUI
	EventTimeTempoTap():pressed(false) {}
	EventTimeTempoTap(bool p): pressed(p) {}
};

class EventTimeBarBeat : public EventBase
{
public:
	int type()
	{
		return int(TIME_BAR_BEAT);
	}
	uint32_t size()
	{
		return sizeof(EventTimeBarBeat);
	}

	int bar;
	int beat;
	EventTimeBarBeat(): bar(0), beat(0) {}
	EventTimeBarBeat(int ba, int be): bar(ba), beat(be) {}
};

class EventLooperClipRequestBuffer : public EventBase
{
public:
	int type()
	{
		return int(LOOPER_REQUEST_BUFFER);
	}
	uint32_t size()
	{
		return sizeof(EventLooperClipRequestBuffer);
	}

	int track;
	int scene;

	// number of floats to contain
	unsigned long numElements;

	// pointer to the new AudioBuffer being passed back
	AudioBuffer* ab;

	EventLooperClipRequestBuffer(): track(0), scene(0), numElements(0), ab(0) {}
	EventLooperClipRequestBuffer(int t, int s, int siz): track(t), scene(s), numElements(siz), ab(0) {}
	EventLooperClipRequestBuffer(int t, int s, AudioBuffer* a): track(t), scene(s), numElements(0), ab(a) {}
};


class EventStateSaveBuffer : public EventBase
{
public:
	int type()
	{
		return int(STATE_SAVE_BUFFER);
	}
	uint32_t size()
	{
		return sizeof(EventStateSaveBuffer);
	}

	int track;
	int scene;
	// pointer to the AudioBuffer to be saved
	AudioBuffer* ab;

	EventStateSaveBuffer(): track(0), scene(0), ab(0) {}
	EventStateSaveBuffer(int t, int s, AudioBuffer* a): track(t), scene(s), ab(a) {}
};

class EventRequestSaveBuffer : public EventBase
{
public:
	int type()
	{
		return int(REQUEST_SAVE_BUFFER);
	}
	uint32_t size()
	{
		return sizeof(EventRequestSaveBuffer);
	}

	int track;
	int scene;

	size_t bufferSize;

	// pointer to the AudioBuffer to be saved
	AudioBuffer* ab;

	EventRequestSaveBuffer(): track(0), scene(0), ab(0) {}
	EventRequestSaveBuffer(int t, int s, size_t si): track(t), scene(s), bufferSize(si), ab(0) {}
	EventRequestSaveBuffer(int t, int s, AudioBuffer* a): track(t), scene(s), ab(a) {}
};

class EventDeallocateBuffer : public EventBase
{
public:
	int type()
	{
		return int(DEALLOCATE_BUFFER);
	}
	uint32_t size()
	{
		return sizeof(EventDeallocateBuffer);
	}

	AudioBuffer* ab;

	EventDeallocateBuffer(): ab(0) {}
	EventDeallocateBuffer(AudioBuffer* a): ab(a) {}
};


// prints the string S in the GUI console
class EventGuiPrint : public EventBase
{
public:
	int type()
	{
		return int(GUI_PRINT);
	}
	uint32_t size()
	{
		return sizeof(EventGuiPrint);
	}

	char stringArray[50];

	EventGuiPrint() {}
	EventGuiPrint(const char* s)
	{
		if ( strlen( s ) > 49 ) {
			// this will be called from an RT context, and should be removed from
			// production code. It is here for the programmer to notice when they
			// are using code which causes too long a message.
			cout << "EventGuiPrint() error! Size of string too long!" << endl;
		} else {
			// move the sting into this event
			strcpy( &stringArray[0], s );
		}
	}
	char* getMessage()
	{
		return &stringArray[0];
	}
};


class EventControllerBindingTarget : public EventBase
{
public:
	int type()
	{
		return int(CONTROLLER_BINDING_TARGET);
	}
	uint32_t size()
	{
		return sizeof(EventControllerBindingTarget);
	}
	char target[50];

	EventControllerBindingTarget() {}
	EventControllerBindingTarget(const char* s)
	{
		if ( strlen( s ) > 49 ) {
			// this will be called from an RT context, and should be removed from
			// production code. It is here for the programmer to notice when they
			// are using code which causes too long a message.
			cout << "EventControllerBindingTarget() error! Size of string too long!" << endl;
		} else {
			// move the sting into this event
			strcpy( &target[0], s );
		}
	}
	char* getMessage()
	{
		return &target[0];
	}
};


#endif // LUPPP_EVENT_H

