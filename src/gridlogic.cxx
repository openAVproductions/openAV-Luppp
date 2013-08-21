
#include "gridlogic.hxx"

#include "jack.hxx"
#include "audiobuffer.hxx"

extern Jack* jack;


const char* StateString[8] = {
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


void GridLogic::pressed( int track, int scene )
{
  //printf("before press state = %s\n", StateString[ int(state[track*NTRACKS + scene]) ] );
  /*
  if ( state[track*NSCENES + scene] == STATE_EMPTY )
    state[track*NSCENES + scene] = STATE_RECORD_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_STOPPED )
    state[track*NSCENES + scene] = STATE_PLAY_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_PLAYING )
    state[track*NSCENES + scene] = STATE_STOP_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_RECORDING )
    state[track*NSCENES + scene] = STATE_STOP_QUEUED;
  
  //printf("after press state = %s\n", StateString[ int(state[track*NSCENES + scene]) ] );
  
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene]);
  */
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
  }
}


void GridLogic::beat()
{
  
  
}
