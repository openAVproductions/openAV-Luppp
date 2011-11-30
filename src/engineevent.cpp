
#include "engineevent.hpp"

#include <string>
#include <cstring>

void EngineEvent::looperPlay           (int id){ia = id; type = EE_LOOPER_PLAY;}
void EngineEvent::looperStop           (int id){ia = id; type = EE_LOOPER_STOP;}
void EngineEvent::looperLoad           (int id, int slot, int buf){ia = id, ib = slot; ic = buf; type = EE_LOOPER_LOAD;}
void EngineEvent::looperWrite          (int id, char* s){ia = id, sa = strdup(s); type = EE_LOOPER_WRITE;}
void EngineEvent::looperRetrigger      (int id) { ia = id; type = EE_LOOPER_RETRIGGER; }
void EngineEvent::setLooperRecord      (int id, int r) { ia = id; ib = r; type = EE_LOOPER_RECORD; }
void EngineEvent::looperSetBeats       (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SET_BEATS; }
void EngineEvent::looperSetInputType   (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SET_INPUT_TYPE; }
void EngineEvent::looperSelectBuffer   (int id, int b) { ia = id; ib = b; type = EE_LOOPER_SELECT_BUFFER; }
void EngineEvent::looperSetQuantization(int id, int b) { ia = id; ib = b; type = EE_LOOPER_QUANTIZATION; }

void EngineEvent::samplerPlay          (int id) { ia = id; type = EE_SAMPLER_PLAY; }
void EngineEvent::samplerStop          (int id) { ia = id; type = EE_SAMPLER_STOP; }
void EngineEvent::samplerSetMode       (int id, int b) { ia = id; ib = b; type = EE_SAMPLER_SET_MODE; }

void EngineEvent::setTrackRec          (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_REC; }
void EngineEvent::setTrackSolo         (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_SOLO; }
void EngineEvent::setTrackMute         (int id, int b) { ia = id; ib = b; type = EE_TRACK_SET_MUTE; }
void EngineEvent::setTrackSelectDevice (int id, int b) { ia = id; ib = b; type = EE_TRACK_SELECT_DEVICE; }
void EngineEvent::setPluginParameter   (int i1, int i2, int i3, float f1) {ia=i1;ib=i2;ic=i3;fa=f1;type=EE_TRACK_SET_PLUGIN_PARAMETER;}
void EngineEvent::setPluginParameterAbsolute(int i1, int i2, int i3, float f1) {ia=i1;ib=i2;ic=i3;fa=f1;type=EE_TRACK_SET_PLUGIN_PARAMETER_ABSOLUTE;}

void EngineEvent::setTrackDeviceActive (int id, int b, int a) { ia = id; ib = b; ic = a; type = EE_TRACK_DEVICE_ACTIVE; }
void EngineEvent::setTrackSetDeviceType(int id, int b, int a) { ia = id; ib = b; ic = a; type = EE_TRACK_SET_DEVICE_TYPE; }

void EngineEvent::setMixerVolume       (int id, float v) { ia = id; fa = v; type = EE_MIXER_VOLUME; }

void EngineEvent::sendAllPan           () { type = EE_SEND_ALL_PAN; }

// never called from a RT context, so strdup is safe
void EngineEvent::rhLoadBuffer         (int id, int slot, char* s){ia = id; ib = slot, sa = strdup(s); type = EE_RH_LOAD_BUFFER; }
void EngineEvent::rhWriteBuffer        (int id, char* s){ia = id; sa = strdup(s); type = EE_RH_WRITE_BUFFER; }
void EngineEvent::rhInsertLv2          (int i , int i2, char* s) { ia = i; ib = i2; sa = strdup(s); type = EE_RH_INSERT_LV2; }
void EngineEvent::setFramesPerBeat     (int i , int i2) { ia = i; ib = i2; type = EE_RH_SET_FRAMES_PER_BEAT; }
