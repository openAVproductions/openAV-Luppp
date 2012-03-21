
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

#ifndef LUPPP_TIME
#define LUPPP_TIME

#include <iostream>

#include <glibmm.h>

#include "automove.hpp"
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
    
    void startAutomoveType(int);
    
    void process(int nframes);
    void processEngineEvent(EngineEvent* e);
  
  private:
    Top* top;
    
    int beat;
    
    bool automoveQueue;
    int automoveStartFrame;
    int automoveDuration;
    AutoMoveType automoveType;
    
    // helper functions for processing queues
    void doEngineEvent(EngineEvent* );
    void doEngineEventList(std::list<EngineEvent*>& list);
    
    std::list<EngineEvent*> q1;
    std::list<EngineEvent*> q4;
    std::list<EngineEvent*> q8;
    std::list<EngineEvent*> q16;
};

#endif
