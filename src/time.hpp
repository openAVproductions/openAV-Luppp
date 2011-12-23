
#ifndef LUPPP_TIME
#define LUPPP_TIME

#include <iostream>

#include <glibmm.h>

#include "statestore.hpp"
#include "engineevent.hpp"

class Top;

// the Time class does all time related sequencing & changing of state
// it seems a good idea to keep all these functions close together, and
// have the Time class operate on the StateStore, as this will reduce the
// spread of Time related code, and hopefully reduce bugs
class Time
{
  public:
    Time(Top*);
    
    void process(int nframes);
    void processEngineEvent(EngineEvent* e);
  
  private:
    Top* top;
    
    int beat;
    
    // helper functions for processing queues
    void doEngineEvent(EngineEvent* );
    void doEngineEventList(std::list<EngineEvent*>& list);
    
    std::list<EngineEvent*> q1;
    std::list<EngineEvent*> q4;
    std::list<EngineEvent*> q8;
    std::list<EngineEvent*> q16;
};

#endif
