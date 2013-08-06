
#include "logic.hxx"

#include "jack.hxx"
extern Jack* jack;

Logic::Logic()
{
  
}

void Logic::tapTempo()
{
  jack->getTimeManager()->tap();
}

void Logic::trackVolume(int t, float v)
{
  //printf( "Logic trackVolume() %i, %f\n", t, v );
  jack->getTrackOutput( t )->setMaster( v );
  jack->getControllerUpdater()->volume( t, v );
}


void Logic::trackRecordArm(int t, bool v)
{
  
}


void Logic::trackSend(int t, int send, float v)
{
  jack->getTrackOutput( t )->setSend( send, v );
  jack->getControllerUpdater()->setTrackSend( t, send, v );
}
