
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
  if ( t < 0 ) // master track
  {
    jack->masterVolume(v);
    jack->getControllerUpdater()->masterVolume( v );
  }
  else
  {
    jack->getTrackOutput( t )->setMaster( v );
    jack->getControllerUpdater()->volume( t, v );
  }
}

void Logic::trackRecordArm(int t, bool v)
{
  
}


void Logic::trackSend(int t, int send, float v)
{
  jack->getTrackOutput( t )->setSend( send, v );
  jack->getControllerUpdater()->setTrackSend( t, send, v );
}

void Logic::looperClipLenght(int t, int s, int l)
{
  jack->getLooper( t )->getClip( s )->setBeats(l);
}


void Logic::looperUseAsTempo(int t, int s)
{
  size_t clipBeats  = jack->getLooper( t )->getClip( s )->getBeats();
  size_t clipFrames = jack->getLooper( t )->getClip( s )->getBufferLenght();
  
  if ( clipBeats > 0 )
  {
    size_t framesPerBeat = clipFrames / clipBeats;
    jack->getTimeManager()->setFpb( framesPerBeat );
  }
  
}
