
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
  size_t l = jack->getLooper( t )->getClip( s )->getBufferLenght();
  printf("lenght = %zu\n", l );
  
  int beats = 1;
  
  int four    = l / 4;
  int eight   = l / 8;
  int sixteen = l / 16;
  int tirty2  = l / 32;
  int sixty4  = l / 64;
  
  printf("%i\n%i\n%i\n%i\n", four, eight, sixteen, tirty2 );
  
  size_t res = l;
  
  while ( res > 22050 / 2 )
  {
    res = l / beats;
    
    if ( res - 22050 < jack->getTimeManager()->getFpb() / 2.f )
    {
      printf("using beats %i\n", beats);
      jack->getTimeManager()->setFpb(res);
    }
    
    beats = beats + beats;
  }
  
  
}
