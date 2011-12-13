
#ifndef LUPPP_ENGINEEVENT
#define LUPPP_ENGINEEVENT

#include <iostream>


enum EngineEventType
{
  EE_INVALID = 0,
  
  EE_MIXER_VOLUME,
  
  EE_LOOPER_RECORD,
  EE_LOOPER_RETRIGGER,
  EE_LOOPER_SET_BEATS,
  EE_LOOPER_SET_INPUT_TYPE,
  EE_LOOPER_LOAD,
  EE_LOOPER_WRITE,
  EE_LOOPER_PLAY,
  EE_LOOPER_STOP,
  EE_LOOPER_SELECT_BUFFER,
  EE_LOOPER_QUANTIZATION,
  
  EE_SAMPLER_PLAY,
  EE_SAMPLER_STOP,
  EE_SAMPLER_SET_MODE,
  
  EE_TRACK_SET_SPEED,
  EE_TRACK_SET_PAN,
  EE_TRACK_SET_REC,
  EE_TRACK_SET_SOLO,
  EE_TRACK_SET_MUTE,
  EE_TRACK_SELECT_DEVICE,
  EE_TRACK_SET_DEVICE_TYPE,
  EE_TRACK_SET_PLUGIN_PARAMETER,
  EE_TRACK_SET_PLUGIN_PARAMETER_ABSOLUTE,
  
  EE_TRACK_DEVICE_ACTIVE,
  
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
    
    EngineEvent() {
      type = EE_INVALID;
      sa = 0;
    }
    
    void looperPlay           (int id);
    void looperStop           (int id);
    void looperLoad           (int id, int slot, int buf);
    void looperWrite          (int id, char* s);
    void looperRetrigger      (int id);
    void setLooperRecord      (int id, int r);
    void looperSetBeats       (int id, int b);
    void looperSetInputType   (int id, int b);
    void looperSelectBuffer   (int id, int b);
    void looperSetQuantization(int id, int b);
    
    void samplerPlay          (int id);
    void samplerStop          (int id);
    void samplerSetMode       (int id, int b);
    
    void setTrackSpeed        (int id, float s);
    void setTrackPan          (int id, float p);
    void setTrackRec          (int id, int b);
    void setTrackSolo         (int id, int b);
    void setTrackMute         (int id, int b);
    void setTrackSelectDevice (int id, int b);
    void setPluginParameter   (int i1, int i2, int i3, float f1);
    void setPluginParameterAbsolute(int i1, int i2, int i3, float f1);
    
    void setTrackDeviceActive (int id, int b, int a);
    void setTrackSetDeviceType(int id, int b, int a);
    
    void setMixerVolume       (int id, float v);
    
    void sendAllPan           ();
    
    void setStateAudioBuffer  (void*);
    void setStateEffect       (int t, int p, void* efctPtr);

    
    // never called from a RT context, so strdup is safe
    void topLoadBuffer        (int id, int slot, char* s);
    void topWriteBuffer       (int id, char* s);
    void topInsertLv2         (int i , int i2, char* s);
    void topSetFramesPerBeat  (int i , int i2);
};

#endif
