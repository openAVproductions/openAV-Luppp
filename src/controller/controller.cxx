
#include "controller.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../controllerupdater.hxx"

extern Jack* jack;

Controller::Controller()
{
  jack->getControllerUpdater()->registerController( this );
}
