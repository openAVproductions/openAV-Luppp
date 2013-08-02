
#include "gridlogic.hxx"

#include "jack.hxx"

extern Jack* jack;

GridLogic::GridLogic()
{
  
}


void GridLogic::pressed( int track, int scene )
{
  state[track*NTRACKS + scene] = STATE_PLAYING;
  jack->getControllerUpdater()->setSceneState(track, scene, STATE_PLAYING);
}


void GridLogic::released( int track, int scene )
{
  jack->getControllerUpdater()->setSceneState(track, scene, state[track*NTRACKS + scene] );
}


void GridLogic::updateState()
{
  
}
