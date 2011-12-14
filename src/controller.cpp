
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
  track = t;
  device = d;
}

int Controller::getTrack()
{
  return track;
}

int Controller::getDevice()
{
  return device;
}
