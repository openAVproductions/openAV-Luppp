
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

#include "controller.hpp"

#include "top.hpp"

using namespace std;

Controller::Controller(Top* t)
{
  //cout << "Controller()" << endl;
  
  top = t;
  
  track = 0;
  device = 0;
}

void Controller::setTrackDevice(int t, int d)
{
  // called from JACK thread, to update GUI
  track = t;
  device = d;
  
  EngineEvent* x = new EngineEvent();
  x->setTrackSelectDevice(t, d);
  top->toGuiQueue.push(x);
}

int Controller::getTrack()
{
  return track;
}

int Controller::getDevice()
{
  return device;
}
