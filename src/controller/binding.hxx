

#ifndef LUPPP_BINDING_H
#define LUPPP_BINDING_H

#include <map>

/// a LupppAction represents the Event type, as from Event.hxx
typedef int LupppAction;

class Binding
{
  public:
    Binding() : status(0), data(0), action(0), active(-1),
                track(-2),scene(-1),send(-1) {}
    
    unsigned char status;
    unsigned char data;
    
    /// the action this binding relates to: this is an integer based on the
    /// event.hxx enumeration of event types
    LupppAction action;
    
    /// arguments to the event: track number, scene number etc
    int active;
    int track;
    int scene;
    int send;
    
    /// maps from Gridlogic::State to MIDI output value from binding
    std::map<int,int> clipStateMap;
};

#endif
