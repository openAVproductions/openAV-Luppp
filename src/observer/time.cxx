
#include "time.hxx"

#include "../jack.hxx"
extern Jack* jack;

#include "../timemanager.hxx"

TimeObserver::TimeObserver()
{
  jack->getTimeManager()->registerObserver( this );
}

