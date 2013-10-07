
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

void Logic::metronomeEnable(bool b)
{
  jack->getMetronome()->setActive(b);
  jack->getControllerUpdater()->metronomeEnable( b );
}

void Logic::masterInputVol( float v )
{
  jack->inputVolume( v );
}

void Logic::masterInputTo( Event::INPUT_TO inputTo, float v)
{
  jack->inputTo( inputTo, v );
}

void Logic::masterReturn( int send, float v )
{
  
  
}

void Logic::masterInputToActive( Event::INPUT_TO inputTo, bool active)
{
  jack->inputToActive( inputTo, active);
}

void Logic::trackVolume(int t, float v)
{
  if ( t == -1 ) // master track
  {
    jack->masterVolume(v);
    jack->getControllerUpdater()->masterVolume( v );
  }
  else if ( t >= 0 && t < NTRACKS )
  {
    jack->getTrackOutput( t )->setMaster( v );
    jack->getControllerUpdater()->volume( t, v );
  }
  else
  {
    LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
  }
}

void Logic::trackRecordArm(int t, bool v)
{
  if ( t >= 0 && t < NTRACKS )
  {
    jack->getTrackOutput( t )->recordArm( v );
    jack->getControllerUpdater()->recordArm( t, v );
  }
  else
  {
    LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
  }
}

void Logic::trackSendActive(int t, int s, bool v)
{
  if ( t >= 0 && t < NTRACKS )
  {
    jack->getTrackOutput( t )->setSendActive( s, v );
    jack->getControllerUpdater()->setTrackSendActive( t, s, v );
  }
  else
  {
    LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
  }
}

void Logic::trackSend(int t, int send, float v)
{
  if ( t >= 0 && t < NTRACKS )
  {
#ifdef DEBUG_LOGIC
    cout << "Logic::trackSend() " << t << " " << send << " " << v << endl;
#endif
    jack->getTrackOutput( t )->setSend( send, v );
    jack->getControllerUpdater()->setTrackSend( t, send, v );
  }
  else
  {
    LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
  }
}

void Logic::looperClipLenght(int t, int s, int l)
{
  if ( t >= 0 && t < NTRACKS )
  {
    jack->getLooper( t )->getClip( s )->setBeats(l);
  }
  else
  {
    LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
  }
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
