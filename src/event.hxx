
#ifndef LUPPP_EVENT_H
#define LUPPP_EVENT_H

#include <stdint.h>

/*
    event.hxx
  
  This file provides declarations for each type of event that the engine uses.
  
*/

namespace Event
{
  enum {
    LOAD_SAMPLE = 0,
    PLAY_SAMPLE,
    MASTER_VOL,
    RECORD,
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

class EventLoadSample : public EventBase
{
  public:
    int type() { return int(LOAD_SAMPLE); }
    uint32_t size() { return sizeof(EventLoadSample); }
    
    AudioBuffer* audioBufferPtr;
    
    EventLoadSample(AudioBuffer* a)
    {
      audioBufferPtr = a;
    }
};

class EventPlaySample : public EventBase
{
  public:
    int type() { return int(PLAY_SAMPLE); }
    uint32_t size() { return sizeof(EventPlaySample); }
    
    int track, bufferID;
    
    EventPlaySample(int t, int id)
    {
      track = t;
      bufferID = id;
    }
};

class EventRecord : public EventBase
{
  public:
    int type() { return int(RECORD); }
    uint32_t size() { return sizeof(EventRecord); }
    
    int track;
    bool record;
    
    EventRecord(int t, bool b)
    {
      track = t;
      record = b;
    }
};


#endif // LUPPP_EVENT_H

