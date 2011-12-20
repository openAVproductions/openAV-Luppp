
#include "controller.hpp"

#include "top.hpp"

using namespace std;

Controller::Controller(Top* t)
{
  cout << "Controller()" << endl;
  
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
