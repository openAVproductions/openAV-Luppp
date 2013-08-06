
#include "gridlogic.hxx"

#include "jack.hxx"

extern Jack* jack;

GridLogic::GridLogic()
{
  
}


void GridLogic::pressed( int track, int scene )
{
  printf("before press state = %s\n", StateString[ int(state[track*NTRACKS + scene]) ] );
  
  if ( state[track*NTRACKS + scene] == STATE_EMPTY )
    state[track*NTRACKS + scene] = STATE_RECORD_QUEUED;
  
  if ( state[track*NTRACKS + scene] == STATE_LOADED )
    state[track*NTRACKS + scene] = STATE_PLAY_QUEUED;
  
  if ( state[track*NTRACKS + scene] == STATE_PLAYING )
    state[track*NTRACKS + scene] = STATE_STOP_QUEUED;
  
  if ( state[track*NTRACKS + scene] == STATE_RECORDING )
    state[track*NTRACKS + scene] = STATE_STOP_QUEUED;
  
  printf("after press state = %s\n", StateString[ int(state[track*NTRACKS + scene]) ] );
  
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NTRACKS + scene]);
}


void GridLogic::released( int track, int scene )
{
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NTRACKS + scene] );
}


void GridLogic::updateState()
{
}


void GridLogic::updateState( int track, int scene )
{
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NTRACKS + scene] );
}


void GridLogic::bar()
{
  printf("GridLogic::bar()\n" );
  
  /// iterate over all clips, if they're set to QUEUED, set to the next state
  for( int i = 0; i < NTRACKS*NSCENES; i++ )
  {
    bool change = false;
    
    if      ( state[i] == STATE_PLAY_QUEUED )
    {
      state[i] = STATE_PLAYING;
      change = true;
    }
    else if ( state[i] == STATE_STOP_QUEUED ) 
    {
      state[i] = STATE_LOADED;
      change = true;
    }
    else if ( state[i] == STATE_RECORD_QUEUED ) 
    {
      state[i] = STATE_RECORDING;
      change = true;
    }
    
    if ( change )
    {
      int track = i / NTRACKS;
      int scene = i - scene;
      updateState( track, scene );
      printf("GridLogic::bar(), updated %i, %i\n", track, scene );
    }
    
  }
}


void GridLogic::beat()
{
  
  
}
