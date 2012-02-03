
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

#ifndef LUPPP_TOP
#define LUPPP_TOP

#include <iostream>

#include <glibmm.h>

#include "rtqueue.hpp"
#include "audioqueue.hpp"

#include "statestore.hpp"

class Controller;
class JackClient;
class GuiStateStore;
class OfflineWorker;

// the Top class is a portal trough which the rest of the program can
// commnunicate. It allows the JACK thread send messages to other threads,
// and vice versa. It also aids as a point where various utility classes
// can communicate
class Top
{
  public:
    Top();
    void initialize();
    void setJackPointer(JackClient* j){jackClient = j;};
    
    int bufferSize;
    int samplerate;
    int bpm;
    int frameNumber;
    
    float speed;
    
    Glib::Mutex scopeVectorMutex;
    std::vector<float> scopeVector;
    std::vector<float> inputScopeVector;
    
    GuiStateStore* guiState;
    Controller* controller; 
    OfflineWorker* offlineWorker;
    StateStore state;
    
    Glib::Dispatcher* guiDispatcher;
    RtQueue toGuiQueue;
    
    RtQueue toEngineQueue;
    
    RtQueue toEngineEmptyEventQueue;
    
    AudioQueue recordAudioQueue;
    
    JackClient* jackClient;
};

#endif
