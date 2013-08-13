
#include "gridlogic.hxx"

#include "jack.hxx"

extern Jack* jack;


const char* StateString[8] = {
  "empty",
  "playing",
  "play queued",
  "loaded",
  "stop queued",
  "recording",
  "record queued"
};

GridLogic::GridLogic()
{
  
}


void GridLogic::pressed( int track, int scene )
{
  //printf("before press state = %s\n", StateString[ int(state[track*NTRACKS + scene]) ] );
  
  if ( state[track*NSCENES + scene] == STATE_EMPTY )
    state[track*NSCENES + scene] = STATE_RECORD_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_LOADED )
    state[track*NSCENES + scene] = STATE_PLAY_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_PLAYING )
    state[track*NSCENES + scene] = STATE_STOP_QUEUED;
  
  if ( state[track*NSCENES + scene] == STATE_RECORDING )
    state[track*NSCENES + scene] = STATE_STOP_QUEUED;
  
  //printf("after press state = %s\n", StateString[ int(state[track*NSCENES + scene]) ] );
  
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene]);
}


void GridLogic::released( int track, int scene )
{
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene] );
}

void GridLogic::load(int track, int scene)
{
  state[track*NSCENES + scene] = STATE_LOADED;
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene] );
}


void GridLogic::updateState()
{
  printf("GridLogic::updateState() stub" );
}


void GridLogic::bar()
{
  printf("GridLogic::bar()\n" );
  
  /// iterate over all clips, if they're set to QUEUED, set to the next state
  for( int i = 0; i < NTRACKS*NSCENES; i++ )
  {
    int track = i / NSCENES;
    int scene = i - track * NSCENES;
    bool change = false;
    
    if      ( state[i] == STATE_PLAY_QUEUED )
    {
      state[i] = STATE_PLAYING;
      jack->getLooper( track )->setRecord( scene, false);
      jack->getLooper( track )->play( scene, true);
      change = true;
    }
    else if ( state[i] == STATE_STOP_QUEUED ) 
    {
      state[i] = STATE_LOADED;
      jack->getLooper( track )->setRecord( scene, false);
      jack->getLooper( track )->play( scene, false);
      change = true;
    }
    else if ( state[i] == STATE_RECORD_QUEUED ) 
    {
      state[i] = STATE_RECORDING;
      jack->getLooper( track )->setRecord( scene, true);
      jack->getLooper( track )->play( scene, false);
      change = true;
    }
    
    if ( change )
    {
      jack->getControllerUpdater()->setSceneState(track, scene, state[track*NSCENES + scene] );
      //printf("GridLogic::bar(), i = %i, track %i, scene %i\n", i, track, scene );
    }
    
  }
}


void GridLogic::beat()
{
  
  
}
