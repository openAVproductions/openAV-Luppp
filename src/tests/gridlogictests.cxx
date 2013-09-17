

#include "../gridlogic.hxx"

#include "../jack.hxx"
#include "../looperclip.hxx"


extern Jack* jack;

#ifdef BUILD_TESTS

#include "../config.hxx"
#include "qunit.hxx"

int GridLogic::runTests()
{
  QUnit::UnitTest qunit( QUnit::normal, true );
  int t = 0;
  int s = 0;
  LooperClip* lc  = jack->getLooper( t )->getClip( s );
  
  /// SCENE LAUNCH
  lc->init();
  jack->getGridLogic()->launchScene( s );
  QUNIT_IS_TRUE( jack->getGridLogic()->getLaunchedScene() == s );
  
  
  /// PAD STATE CHECKS
  // empty -> recording
  lc->init();
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_EMPTY );
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORD_QUEUED );
  jack->getGridLogic()->bar();
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORDING );
  
  float buffer[64];
  lc->record(64, &buffer[0], &buffer[0]);
  
  // recording -> playing
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
  jack->getGridLogic()->bar();
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );
  // playing -> stopped
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
  jack->getGridLogic()->bar();
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );
  // stopped -> playing
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
  jack->getGridLogic()->bar();
  QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );
  
  
  return qunit.errors();
}

#endif // BUILD_TESTS

