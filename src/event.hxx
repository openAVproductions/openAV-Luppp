
#ifndef LUPPP_EVENT_H
#define LUPPP_EVENT_H

#include <stdint.h>

/*
    event.hxx
  
  This file provides declarations for each type of event that the engine uses.
  
*/

#include "looper.hxx"

namespace Event
{
  enum {
    MASTER_VOL,
    RECORD,
    
    TRACK_SIGNAL_LEVEL,
    
    LOOPER_LOAD,
    LOOPER_STATE,
    LOOPER_PROGRESS,
    LOOPER_LOOP_LENGTH,
    
    METRONOME_ACTIVE,
    
    TIME_BPM,
    TIME_TEMPO_TAP,
    
    GUI_PRINT,
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

class EventMasterVol : public EventBase
{
  public:
    int type() { return int(MASTER_VOL); }
    uint32_t size() { return sizeof(EventMasterVol); }
    float vol;
    
    EventMasterVol(float v)
    {
      vol = v;
    }
};

class EventLooperState : public EventBase
{
  public:
    int type() { return int(LOOPER_STATE); }
    uint32_t size() { return sizeof(EventLooperState); }
    
    int track;
    int scene;
    Looper::State state;
    EventLooperState(){}
    EventLooperState(int t, int sc, Looper::State s) : track(t), scene(sc), state(s){}
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
    float scale; // multiply length by this
    EventLooperLoopLength(){}
    EventLooperLoopLength(int t, float s) : track(t), scale(s){}
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
    
    // no data needed, the event itself is an "event",
    // jack measures time intervals in frames & does math
    EventTimeTempoTap(){}
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

