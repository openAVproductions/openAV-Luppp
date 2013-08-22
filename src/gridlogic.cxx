
#include "gridlogic.hxx"

#include "jack.hxx"
#include "audiobuffer.hxx"

extern Jack* jack;


const char* GridLogic::StateString[8] = {
  "empty",
  "playing",
  "play queued",
  "stopped",
  "stop queued",
  "recording",
  "record queued"
};

GridLogic::GridLogic()
{
  for( int i = 0; i < NTRACKS*NSCENES; i++ )
  {
    //state[i] = STATE_EMPTY;
  }
}


void GridLogic::launchScene( int scene )
{
  for(unsigned int t = 0; t < NTRACKS; t++ )
  {
    for(unsigned int s = 0; s < NSCENES; s++ )
    {
      LooperClip* lc = jack->getLooper( t )->getClip( s );
      if ( s == scene )
        lc->queuePlay();
      //else
      //lc->queueStop();
    }
    
    
  }
  
  /*
  for(unsigned int s = 0; s < NSCENES; s++ )
  {
    jack->getControllerUpdater()->setSceneState( -1, s, s );
  }
  */
}

void GridLogic::pressed( int track, int scene )
{
  LooperClip* lc = jack->getLooper( track )->getClip( scene );
  GridLogic::State s = lc->getState();
  
  printf("before press state = %s\n", StateString[ int(s) ] );
  
  if ( s == STATE_EMPTY )
    lc->queueRecord();
  
  if ( s == STATE_STOPPED )
    lc->queuePlay();
  
  if ( s == STATE_PLAYING )
    lc->queueStop();
  
  if ( s == STATE_RECORDING )
    lc->queueStop();
  
  
  s = lc->getState();
  
  printf("after press state = %s\n", StateString[ int(s) ] );
  
  jack->getControllerUpdater()->setSceneState(track, scene, s );
}


void GridLogic::released( int track, int scene )
{
  //jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene] );
}

void GridLogic::load(int track, int scene, AudioBuffer* ab)
{
  jack->getLooper( track )->getClip( scene )->load( ab );
  //jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene] );
}


void GridLogic::updateState()
{
  //printf("GridLogic::updateState() stub" );
}


void GridLogic::bar()
{
  EventGuiPrint e( "GridLogic::bar()\n" );
  writeToGuiRingbuffer( &e );
  
  /// iterate over all clips, if they're set to QUEUED, set to the next state
  for( int i = 0; i < NTRACKS*NSCENES; i++ )
  {
    int track = i / NSCENES;
    int scene = i - track * NSCENES;
    jack->getLooper( track )->getClip( scene )->bar();
    
    GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
    
    if ( s != STATE_EMPTY )
    {
      printf("%i, %i:after bar() state = %s\n", track, scene, StateString[ int(s) ] );
    }
  }
}


void GridLogic::beat()
{
  
  
}
