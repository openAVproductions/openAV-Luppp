
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

#ifndef LUPPP_ENGINEEVENT
#define LUPPP_ENGINEEVENT

#include <iostream>


enum EngineEventType
{
  EE_INVALID = 0,
  
  EE_BPM,
  
  EE_SCENE_NUMBER,
  
  EE_MIXER_VOLUME,
  
  EE_AUTOMOVE_TYPE,
  EE_AUTOMOVE_PROGRESS,
  
  EE_ADD_TRACK,
  EE_SET_TRACK_SOURCE,
  EE_ADD_TRACK_POINTERS,
  
  EE_LOOPER_RECORD,
  EE_LOOPER_RETRIGGER,
  EE_LOOPER_SET_BEATS,
  EE_LOOPER_SET_INPUT_TYPE,
  EE_LOOPER_LOAD,
  EE_LOOPER_WRITE,
  EE_LOOPER_PLAY,
  EE_LOOPER_STOP,
  EE_LOOPER_PROGRESS,
  EE_LOOPER_SELECT_BUFFER,
  EE_LOOPER_QUANTIZATION,
  
  EE_SAMPLER_PLAY,
  EE_SAMPLER_STOP,
  EE_SAMPLER_SET_MODE,
  
  EE_TRACK_RMS,
  
  EE_TRACK_SET_SEND_VOLUME,
  EE_TRACK_SET_SPEED,
  EE_TRACK_SET_PAN,
  EE_TRACK_SET_REC,
  EE_TRACK_SET_SOLO,
  EE_TRACK_SET_MUTE,
  EE_TRACK_SELECT_DEVICE,
  EE_TRACK_SET_DEVICE_TYPE,
  EE_TRACK_SET_PLUGIN_PARAMETER,
  EE_TRACK_SET_PLUGIN_GLOBAL_UNIT,
  EE_TRACK_SET_PLUGIN_PARAMETER_ABSOLUTE,
  
  EE_TRACK_DEVICE_ACTIVE,
  
  EE_TRACK_BACKTRACK_TAKE_SNAPSHOT,
  EE_TRACK_BACKTRACK_RESTORE_SNAPSHOT,
  
  EE_SEND_OSC,
  
  EE_SEND_ALL_PAN,
  
  EE_RH_LOAD_BUFFER,
  EE_RH_WRITE_BUFFER,
  
  EE_RH_INSERT_LV2,
  
  EE_STATE_NEW_EFFECT,
  EE_STATE_SET_AUDIO_BUFFER,
  
  EE_RH_SET_FRAMES_PER_BEAT,
};


class EngineEvent
{
  public:
    EngineEventType type;
    int ia, ib, ic;
    float fa, fb ,fc;
    char* sa;
    void* vPtr;
    void* vPtr2;
    void* vPtr3;
    void* vPtr4;
    
    EngineEvent() {
      //std::cout << "EngineEvent() CONSTRUCTOR" << std::endl;
      type = EE_INVALID;
      sa = 0;
    }
    ~EngineEvent() {
      //std::cout << "~EngineEvent() DESTRUCTOR" << std::endl;
    }
    
    void addTrack             (int newId);
    void setBpm               (int bpm);
    
    void setSceneNumber       (int scene);
    
    void setAutomoveType      (int track, int type);
    void setAutomoveProgress  (int track, float progress);
    
    void sendAddTrackPointers (void*,void*,void*,void*);
    
    void looperPlay           (int id);
    void looperStop           (int id);
    void looperLoad           (int id, int slot, int buf);
    void looperWrite          (int id, char* s);
    void looperRetrigger      (int id);
    void looperSetBeats       (int id, int b);
    void looperProgress       (int id, float);
    void looperSetInputType   (int id, int b);
    void looperSelectBuffer   (int id, int b);
    void looperSetQuantization(int id, int b);
    void setLooperRecord      (int id, int, int r);
    
    void samplerPlay          (int id);
    void samplerStop          (int id);
    void samplerSetMode       (int id, int b);
    
    void setTrackSend         (int id, int send, float v);
    void setTrackSpeed        (int id, float s);
    void setTrackPan          (int id, float p);
    void setTrackRec          (int id, int b);
    void setTrackSolo         (int id, int b);
    void setTrackMute         (int id, int b);
    void setTrackSelectDevice (int id, int b);
    void setTrackBacktrackTake(int id);
    void setTrackBacktrackRestore(int id);
    void setPluginParameter   (int i1, int i2, int i3, float f1);
    void setPluginParameterAbsolute(int i1, int i2, int i3, float f1);
    
    void setPluginGlobalUnit  (int UID, int onOff);
    
    void setTrackRms          (int id, float rms);
    void setTrackSource       (int id, void* source);
    void setTrackDeviceActive (int id, int a);
    void setTrackSetDeviceType(int id, int b, int a);
    
    void setMixerVolume       (int id, float v);
    
    void sendAllPan           ();
    
    void setStateAudioBuffer  (void*);
    void setStateEffect       (int UID, int track, int pos, int type, void* efctPtr, void* efctStatePtr);

    
    // never called from a RT context, so strdup is safe
    void topLoadBuffer        (int id, int slot, char* s);
    void topWriteBuffer       (int id, char* s);
    void topInsertLv2         (int i , int i2, char* s);
    void topSetFramesPerBeat  (int i , int i2);
};

#endif
