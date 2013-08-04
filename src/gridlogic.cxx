
#include "gridlogic.hxx"

#include "jack.hxx"

extern Jack* jack;

GridLogic::GridLogic()
{
  
}


void GridLogic::pressed( int track, int scene )
{
  state[track*NTRACKS + scene] = STATE_PLAYING;
  jack->getControllerUpdater()->setSceneState(track, scene, STATE_PLAY_QUEUED);
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
      int scene = i % NTRACKS;
      int track = i - scene;
      updateState( track, scene );
      printf("GridLogic::bar(), updated %i, %i\n", track, scene );
    }
    
  }
}


void GridLogic::beat()
{
  
  
}
