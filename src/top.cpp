
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

#include "top.hpp"

#include "settings.hpp"
#include "controller.hpp"
#include "offlineworker.hpp"

using namespace std;

Top::Top() :
  state(this)
{
  
  bpm = 150;
  cout << "Top()  BPM = " << bpm << endl;
  
  samplerate = -1;
  bufferSize = -1;
  frameNumber = -1;
  
  settings = new Settings(this);
  controller = new Controller(this);
  offlineWorker = new OfflineWorker(this);
  guiDispatcher = new Glib::Dispatcher();
  
  // global speed
  speed = 1.f;
}

// this function is called from JackClient(), and serve the purpose of
// initializing variables *after* the bufferSize is known. Primary function
// is the scope view in the GUI / live waveform passing
void Top::initialize()
{
  cout << "Top::initialize() BufferSize = " << bufferSize << endl;
  
  scopeVector.resize(bufferSize);
  inputScopeVector.resize(bufferSize);
  
}
