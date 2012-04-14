
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "engineevent.hpp"

#include <string>
#include <cstring>

void EngineEvent::addTrack             (int newId){ ia = newId; type = EE_ADD_TRACK;}
void EngineEvent::setBpm               (int bpm){ ia = bpm; type = EE_BPM;}
void EngineEvent::setBeat              (int beat){ ia = beat; type = EE_BEAT;}

void EngineEvent::setSceneNumber       (int scene){ ia = scene; type = EE_SCENE_NUMBER;}

void EngineEvent::sendAddTrackPointers (void* p1,void* p2,void* p3,void* p4){ vPtr = p1;vPtr2=p2;vPtr3= p3;vPtr4= p4; type = EE_ADD_TRACK_POINTERS;}

void EngineEvent::setAutomoveType      (int track, int automovetype){ia = track; ib = automovetype; type = EE_AUTOMOVE_TYPE;}
void EngineEvent::setAutomoveProgress  (int track, float prog){ia = track; fa = prog; type = EE_AUTOMOVE_PROGRESS;}

void EngineEvent::looperPlay           (int id){ia = id; type = EE_LOOPER_PLAY;}
void EngineEvent::looperStop           (int id){ia = id; type = EE_LOOPER_STOP;}
void EngineEvent::looperLoad           (int id, int slot, int buf){ia = id, ib = slot; ic = buf; type = EE_LOOPER_LOAD;}
void EngineEvent::looperWrite          (int id, char* s){ia = id, sa = strdup(s); type = EE_LOOPER_WRITE;}
void EngineEvent::looperRetrigger      (int id) { ia = id; type = EE_LOOPER_RETRIGGER; }
void EngineEvent::setLooperRecord      (int id, int block, int rec) { ia = id; ib = block; ic = rec; type = EE_LOOPER_RECORD; }
void EngineEvent::looperSetBeats       (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SET_BEATS; }
void EngineEvent::looperProgress       (int id, float p) { ia = id; fa = p; type = EE_LOOPER_PROGRESS; }
void EngineEvent::looperSetInputType   (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SET_INPUT_TYPE; }
void EngineEvent::looperSelectBuffer   (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SELECT_BUFFER; }
void EngineEvent::looperSetQuantization(int id, int b) { ia = id; ib = b; type = EE_LOOPER_QUANTIZATION; }

void EngineEvent::samplerPlay          (int id) { ia = id; type = EE_SAMPLER_PLAY; }
void EngineEvent::samplerStop          (int id) { ia = id; type = EE_SAMPLER_STOP; }
void EngineEvent::samplerSetMode       (int id, int b) { ia = id; ib = b; type = EE_SAMPLER_SET_MODE; }

void EngineEvent::setTrackSend         (int id, int send, float v) { ia = id; ib = send; fa = v; type = EE_TRACK_SET_SEND_VOLUME;}
void EngineEvent::setTrackSpeed        (int id, float v) { ia = id; fa = v; type = EE_TRACK_SET_SPEED;}
void EngineEvent::setTrackPan          (int id, float v) { ia = id; fa = v; type = EE_TRACK_SET_PAN;  }

void EngineEvent::setTrackBacktrackTake(int id) { ia = id; type = EE_TRACK_BACKTRACK_TAKE_SNAPSHOT; }
void EngineEvent::setTrackBacktrackRestore(int id) { ia = id; type = EE_TRACK_BACKTRACK_RESTORE_SNAPSHOT; }

void EngineEvent::setTrackRms          (int id, float rms) { ia = id; fa = rms; type = EE_TRACK_RMS; }
void EngineEvent::setTrackRec          (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_REC; }
void EngineEvent::setTrackSolo         (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_SOLO; }
void EngineEvent::setTrackMute         (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_MUTE; }
void EngineEvent::setTrackSource       (int id, void* s) { ia = id; vPtr = s; type = EE_SET_TRACK_SOURCE;}
void EngineEvent::setTrackSelectDevice (int id, int b) { ia = id; ib = b; type = EE_TRACK_SELECT_DEVICE; }
void EngineEvent::setPluginParameter   (int i1, int i2, int i3, float f1) {ia=i1;ib=i2;ic=i3;fa=f1;type=EE_TRACK_SET_PLUGIN_PARAMETER;}
void EngineEvent::setPluginParameterAbsolute(int i1, int i2, int i3, float f1) {ia=i1;ib=i2;ic=i3;fa=f1;type=EE_TRACK_SET_PLUGIN_PARAMETER_ABSOLUTE;}

void EngineEvent::setPluginGlobalUnit  (int UID, int onOff){ia=UID;ib=onOff;type=EE_TRACK_SET_PLUGIN_GLOBAL_UNIT;}

void EngineEvent::setTrackDeviceActive (int id, int a) { ia = id; ib = a; type = EE_TRACK_DEVICE_ACTIVE; }
void EngineEvent::setTrackSetDeviceType(int id, int b, int a) { ia = id; ib = b; ic = a; type = EE_TRACK_SET_DEVICE_TYPE; }

void EngineEvent::setMixerVolume       (int id, float v) { ia = id; fa = v; type = EE_MIXER_VOLUME; }
void EngineEvent::setMixerReturnVolume (int id, float v) { ia = id; fa = v; type = EE_MIXER_RETURN_VOLUME; }

void EngineEvent::sendAllPan           () { type = EE_SEND_ALL_PAN; }


void EngineEvent::setStateEffect       (int UID, int track, int pos, int et, void* efctPtr, void* efctStatePtr){ ia = UID; ib = track; ic = pos; fa = et; vPtr = efctPtr; vPtr2 = efctStatePtr; type = EE_STATE_NEW_EFFECT; }
void EngineEvent::setStateAudioBuffer  (void* bufPtr) { vPtr = (void*) bufPtr; type = EE_STATE_SET_AUDIO_BUFFER; }

// never called from a RT context, so strdup is safe
void EngineEvent::topLoadBuffer        (int id, int slot, char* s){ia = id; ib = slot, sa = strdup(s); type = EE_RH_LOAD_BUFFER; }
void EngineEvent::topWriteBuffer       (int id, char* s){ia = id; sa = strdup(s); type = EE_RH_WRITE_BUFFER; }
void EngineEvent::topInsertLv2         (int i , int i2, char* s) { ia = i; ib = i2; sa = strdup(s); type = EE_RH_INSERT_LV2; }
void EngineEvent::topSetFramesPerBeat  (int i , int i2) { ia = i; ib = i2; type = EE_RH_SET_FRAMES_PER_BEAT; }
