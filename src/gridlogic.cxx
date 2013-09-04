
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
  sceneLaunch = 0;
}

int GridLogic::getLaunchedScene()
{
  return sceneLaunch;
}


void GridLogic::launchScene( int scene )
{
  for(unsigned int t = 0; t < NTRACKS; t++ )
  {
    for(int s = 0; s < NSCENES; s++ )
    {
      LooperClip* lc = jack->getLooper( t )->getClip( s );
      if ( s == scene )
      {
        lc->queuePlay();
        jack->getControllerUpdater()->setSceneState( t, s, lc->getState() );
      }
      else
      {
        bool current = lc->playing();
        lc->queueStop();
        bool next    = lc->playing();
        if ( current != next )
        {
          jack->getControllerUpdater()->setSceneState( t, s, lc->getState() );
        }
      }
    }
  }
  
  sceneLaunch = scene;
  
  jack->getControllerUpdater()->launchScene( scene );
}

void GridLogic::pressed( int track, int scene )
{
  // get the clip, do the "press" action based on current state.
  LooperClip* lc  = jack->getLooper( track )->getClip( scene );
  TrackOutput* to = jack->getTrackOutput( track );
  GridLogic::State s = lc->getState();

#ifdef DEBUG_CLIP
  printf("GridLogic::pressed() before press state = %s\n", StateString[ int(scene) ] );
#endif 
  
  if ( to->recordArm() && !lc->recording() )
  {
    lc->queueRecord();
    to->recordArm(false);
  }
  else
  {
    if ( s == STATE_EMPTY )
      lc->queueRecord();
    
    if ( s == STATE_STOPPED )
      lc->queuePlay();
    
    if ( s == STATE_PLAYING )
      lc->queueStop();
    
    if ( s == STATE_RECORDING )
      lc->queuePlay();
    
    if ( s == STATE_PLAY_QUEUED )
      lc->queueStop();
    
    if ( s == STATE_STOP_QUEUED )
      lc->queuePlay();
  }
  
  s = lc->getState();
#ifdef DEBUG_CLIP
  printf("GridLogic::pressed() after press state = %s\n", StateString[ int(s) ] );
#endif
  jack->getControllerUpdater()->setSceneState(track, scene, s );
}


void GridLogic::released( int track, int scene )
{
  GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
  jack->getControllerUpdater()->setSceneState(track, scene, s );
}

void GridLogic::load(int track, int scene, AudioBuffer* ab)
{
  jack->getLooper( track )->getClip( scene )->load( ab );
  GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
  jack->getControllerUpdater()->setSceneState(track, scene, s );
}


void GridLogic::updateState()
{
  //printf("GridLogic::updateState() stub" );
}


void GridLogic::bar()
{
#ifdef DEBUG_CLIP
  EventGuiPrint e( "GridLogic::bar()\n" );
  writeToGuiRingbuffer( &e );
#endif
  
  /// iterate over all clips, if they're set to QUEUED, set to the next state
  for( int i = 0; i < NTRACKS*NSCENES; i++ )
  {
    int track = i / NSCENES;
    int scene = i - track * NSCENES;
    jack->getLooper( track )->getClip( scene )->bar();

#ifdef DEBUG_CLIP
    GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
    if ( s != STATE_EMPTY )
    {
      //printf("%i, %i:after bar() state = %s\n", track, scene, StateString[ int(s) ] );
    }
#endif
  }
}


void GridLogic::beat()
{
  
  
}
