
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

using namespace std;

namespace Event
{
  enum SEND_TYPE
  {
    SEND_REV = 0,
    SEND_SIDE,
    SEND_POST,
  };
  enum RETURN_TYPE
  {
    RETURN_REV = 0,
  };
  enum INPUT_TO
  {
    INPUT_TO_SEND = 0,
    INPUT_TO_MIX,
    INPUT_TO_XSIDE,
    INPUT_TO_SIDE_KEY,
  };
  
  enum {
    MASTER_VOL,
    MASTER_INPUT_VOL,
    MASTER_INPUT_TO,
    MASTER_INPUT_TO_ACTIVE,
    MASTER_RETURN,
    RECORD,
    
    STATE_SAVE,       // save action
    STATE_RESET, // reset all state
    STATE_SAVE_FINISH,// save action finished, flush metadata to disk
    STATE_SAVE_BUFFER,// save an individual AudioBuffer* to disk
    
    REQUEST_SAVE_BUFFER, // gets an audioBuffer of a certain size
    
    GRID_EVENT, // press / release events
    GRID_STATE, // state of one block
    GRID_LAUNCH_SCENE,
    
    TRACK_SEND,
    TRACK_SIGNAL_LEVEL,
    TRACK_VOLUME,
    TRACK_RECORD_ARM,
    
    FX_REVERB,
    
    LOOPER_LOAD,
    LOOPER_STATE,
    LOOPER_PROGRESS,
    LOOPER_LOOP_LENGTH,
    LOOPER_LOOP_USE_AS_TEMPO,
    
    METRONOME_ACTIVE,
    
    TIME_BPM,
    TIME_BAR_BEAT,
    TIME_TEMPO_TAP,
    
    GUI_PRINT,
    
    LOOPER_REQUEST_BUFFER,
    DEALLOCATE_BUFFER,
    
    SAMPLERATE,
    
    QUIT,
  };
};

using namespace Event;

class AudioBuffer;

class EventBase
{
  public:
    virtual ~EventBase() {}
    
    virtual int type() = 0;
    virtual uint32_t size() = 0;
};

class EventQuit : public EventBase
{
  public:
    int type() { return int(QUIT); }
    uint32_t size() { return sizeof(EventQuit); }
    EventQuit(){}
};

class EventSamplerate : public EventBase
{
  public:
    int type() { return int(SAMPLERATE); }
    uint32_t size() { return sizeof(EventSamplerate); }
    int samplerate;
    EventSamplerate(int sr = 0): samplerate(sr){}
};

class EventMasterInputTo : public EventBase
{
  public:
    int type() { return int(MASTER_INPUT_TO); }
    uint32_t size() { return sizeof(EventMasterInputTo); }
    
    INPUT_TO place;
    float value;
    EventMasterInputTo() : value(-1){}
    EventMasterInputTo(INPUT_TO p, float v) : place(p), value(v){}
};

class EventMasterInputToActive : public EventBase
{
  public:
    int type() { return int(MASTER_INPUT_TO_ACTIVE); }
    uint32_t size() { return sizeof(EventMasterInputToActive); }
    
    INPUT_TO place;
    bool active;
    EventMasterInputToActive() : active(false){}
    EventMasterInputToActive(INPUT_TO p, bool a) : place(p), active(a){}
};

class EventMasterVol : public EventBase
{
  public:
    int type() { return int(MASTER_VOL); }
    uint32_t size() { return sizeof(EventMasterVol); }
    float vol;
    EventMasterVol(float v = 0) : vol(v){}
};

class EventMasterInputVol : public EventBase
{
  public:
    int type() { return int(MASTER_INPUT_VOL); }
    uint32_t size() { return sizeof(EventMasterInputVol); }
    float vol;
    EventMasterInputVol(float v = 0) : vol(v){}
};

class EventMasterReturn : public EventBase
{
  public:
    int type() { return int(MASTER_RETURN); }
    uint32_t size() { return sizeof(EventMasterReturn); }
    
    RETURN_TYPE ret;
    float value;
    
    EventMasterReturn(){};
    EventMasterReturn(RETURN_TYPE s, float v): ret(s), value(v){}
};

class EventTrackVol : public EventBase
{
  public:
    int type() { return int(TRACK_VOLUME); }
    uint32_t size() { return sizeof(EventTrackVol); }
    
    int track;
    float vol;
    
    EventTrackVol(){};
    EventTrackVol(int t, float v)
    {
      track = t;
      vol = v;
    }
};

class EventTrackRecordArm : public EventBase
{
  public:
    int type() { return int(TRACK_RECORD_ARM); }
    uint32_t size() { return sizeof(EventTrackRecordArm); }
    
    int track;
    float recordArm;
    
    EventTrackRecordArm(){};
    EventTrackRecordArm(int t, bool r)
    {
      track = t;
      recordArm = r;
    }
};

class EventStateSave : public EventBase
{
  public:
    int type() { return int(STATE_SAVE); }
    uint32_t size() { return sizeof(EventStateSave); }
    
    EventStateSave() {}
};

class EventStateReset : public EventBase
{
  public:
    int type() { return int(STATE_RESET); }
    uint32_t size() { return sizeof(EventStateReset); }
    
    EventStateReset() {}
};

class EventStateSaveFinish : public EventBase
{
  public:
    int type() { return int(STATE_SAVE_FINISH); }
    uint32_t size() { return sizeof(EventStateSaveFinish); }
    
    EventStateSaveFinish(){};
};

class EventGridEvent : public EventBase
{
  public:
    int type() { return int(GRID_EVENT); }
    uint32_t size() { return sizeof(EventGridEvent); }
    
    int track;
    int scene;
    bool pressed;
    
    EventGridEvent(){};
    EventGridEvent(int t, int s, bool p): track(t), scene(s), pressed(p) {}
};

class EventGridState : public EventBase
{
  public:
    int type() { return int(GRID_STATE); }
    uint32_t size() { return sizeof(EventGridState); }
    
    int track;
    int scene;
    GridLogic::State state;
    
    EventGridState(){};
    EventGridState(int t, int s, GridLogic::State st): track(t), scene(s), state(st) {}
};

class EventGridLaunchScene : public EventBase
{
  public:
    int type() { return int(GRID_LAUNCH_SCENE); }
    uint32_t size() { return sizeof(EventGridLaunchScene); }
    
    int scene;
    
    EventGridLaunchScene(){};
    EventGridLaunchScene(int s): scene(s){}
};

class EventFxReverb : public EventBase
{
  public:
    int type() { return int(FX_REVERB); }
    uint32_t size() { return sizeof(EventFxReverb); }
    
    bool enable;
    
    float rtSize;
    float wet;
    float damping;
    
    EventFxReverb(){};
    EventFxReverb(bool e, float s, float w, float d): enable(e), rtSize(s), wet(w), damping(d) {}
};

class EventTrackSend : public EventBase
{
  public:
    int type() { return int(TRACK_SEND); }
    uint32_t size() { return sizeof(EventTrackSend); }
    
    int track;
    SEND_TYPE send;
    float value;
    
    EventTrackSend(){};
    EventTrackSend(int t, SEND_TYPE s, float v): track(t), send(s), value(v){}
};

class EventLooperState : public EventBase
{
  public:
    int type() { return int(LOOPER_STATE); }
    uint32_t size() { return sizeof(EventLooperState); }
    
    int track;
    int scene;
    //Looper::State state;
    
    EventLooperState(){}
    //EventLooperState(int t, int sc, Looper::State s) : track(t), scene(sc), state(s){}
};

class EventLooperProgress : public EventBase
{
  public:
    int type() { return int(LOOPER_PROGRESS); }
    uint32_t size() { return sizeof(EventLooperProgress); }
    
    int track;
    float progress;
    EventLooperProgress(){}
    EventLooperProgress(int t, float p) : track(t), progress(p) {}
};

class EventLooperLoopLength : public EventBase
{
  public:
    int type() { return int(LOOPER_LOOP_LENGTH); }
    uint32_t size() { return sizeof(EventLooperLoopLength); }
    
    int track;
    int scene;
    int beats;
    EventLooperLoopLength(){}
    EventLooperLoopLength(int t, int s, int b) : track(t), scene(s), beats(b) {}
};

class EventLooperUseAsTempo : public EventBase
{
  public:
    int type() { return int(LOOPER_LOOP_USE_AS_TEMPO); }
    uint32_t size() { return sizeof(EventLooperUseAsTempo); }
    
    int track;
    int scene;
    int beats;
    EventLooperUseAsTempo(){}
    EventLooperUseAsTempo(int t, int s) : track(t), scene(s){}
};

class EventLooperLoad : public EventBase
{
  public:
    int type() { return int(LOOPER_LOAD); }
    uint32_t size() { return sizeof(EventLooperLoad); }
    
    int track;
    int clip;
    void* audioBuffer;
    
    EventLooperLoad(){}
    EventLooperLoad(int t, int c, void* ab) : track(t), clip(c), audioBuffer(ab)
    {
      cout << "ab ptr = " << audioBuffer << endl;
    }
};

class EventMetronomeActive : public EventBase
{
  public:
    int type() { return int(METRONOME_ACTIVE); }
    uint32_t size() { return sizeof(EventMetronomeActive); }
    
    bool active;
    
    EventMetronomeActive() : active(false) {}
    EventMetronomeActive(bool a) : active(a) {}
};

class EventTimeBPM : public EventBase
{
  public:
    int type() { return int(TIME_BPM); }
    uint32_t size() { return sizeof(EventTimeBPM); }
    
    float bpm;
    
    EventTimeBPM(){}
    EventTimeBPM(float b) : bpm(b) {}
};

class EventTrackSignalLevel : public EventBase
{
  public:
    int type() { return int(TRACK_SIGNAL_LEVEL); }
    uint32_t size() { return sizeof(EventTrackSignalLevel); }
    
    int track;
    float left;
    float right;
    
    EventTrackSignalLevel(){}
    EventTrackSignalLevel(int t, float l,float r) : track(t), left(l), right(r) {}
};

class EventTimeTempoTap : public EventBase
{
  public:
    int type() { return int(TIME_TEMPO_TAP); }
    uint32_t size() { return sizeof(EventTimeTempoTap); }
    
    bool pressed; // only used to turn on / off the button in GUI
    EventTimeTempoTap():pressed(false){}
    EventTimeTempoTap(bool p): pressed(p) {}
};

class EventTimeBarBeat : public EventBase
{
  public:
    int type() { return int(TIME_BAR_BEAT); }
    uint32_t size() { return sizeof(EventTimeBarBeat); }
    
    int bar;
    int beat;
    EventTimeBarBeat(): bar(0), beat(0){}
    EventTimeBarBeat(int ba, int be): bar(ba), beat(be) {}
};

class EventLooperClipRequestBuffer : public EventBase
{
  public:
    int type() { return int(LOOPER_REQUEST_BUFFER); }
    uint32_t size() { return sizeof(EventLooperClipRequestBuffer); }
    
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
    int type() { return int(STATE_SAVE_BUFFER); }
    uint32_t size() { return sizeof(EventStateSaveBuffer); }
    
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
    int type() { return int(REQUEST_SAVE_BUFFER); }
    uint32_t size() { return sizeof(EventRequestSaveBuffer); }
    
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
    int type() { return int(DEALLOCATE_BUFFER); }
    uint32_t size() { return sizeof(EventDeallocateBuffer); }
    
    AudioBuffer* ab;
    
    EventDeallocateBuffer(): ab(0) {}
    EventDeallocateBuffer(AudioBuffer* a): ab(a) {}
};


// prints the string S in the GUI console
class EventGuiPrint : public EventBase
{
  public:
    int type() { return int(GUI_PRINT); }
    uint32_t size() { return sizeof(EventGuiPrint); }
    
    char stringArray[50];
    
    EventGuiPrint(){}
    EventGuiPrint(const char* s)
    {
      if ( strlen( s ) > 50 )
      {
        // this will be called from an RT context, and should be removed from
        // production code. It is here for the programmer to notice when they
        // are using code which causes too long a message.
        cout << "EventGuiPrint() error! Size of string too long!" << endl;
      }
      else
      {
        // move the sting into this event
        strcpy( &stringArray[0], s );
      }
    }
    char* getMessage()
    {
      return &stringArray[0];
    }
};

#endif // LUPPP_EVENT_H

