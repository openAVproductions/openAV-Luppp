
#include "controller.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../controllerupdater.hxx"

extern Jack* jack;

int Controller::privateID = 0;

Controller::Controller()
{
  ID = privateID++;
}

void Controller::reset()
{
}

int Controller::getID()
{
  return ID;
}
