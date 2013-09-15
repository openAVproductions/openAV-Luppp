
#include "../gridlogic.hxx"

#include "../jack.hxx"
#include "../looperclip.hxx"

extern Jack* jack;

#ifdef BUILD_TESTS
#define CATCH_CONFIG_RUNNER
#include "catch.hxx"
#endif

#ifdef BUILD_TESTS
int GridLogic::runTests()
{
  char* const tmp = "-s";
  return Catch::Session().run( 1, &tmp );
}

TEST_CASE( "Gridlogic press events", "[gridlogic]" )
{
  int t = 0;
  int s = 0;
  LooperClip* lc  = jack->getLooper( t )->getClip( s );
  
  for(int t = 0; t < NTRACKS; t++)
  {
    lc->init();
    GridLogic::State s1 = lc->getState();
    REQUIRE( lc->getState() == GridLogic::STATE_EMPTY );
    jack->getGridLogic()->launchScene( s );
    
    REQUIRE( lc->getQueuePlay() == false );
    jack->getGridLogic()->bar();
    REQUIRE( lc->getQueuePlay() == true );
    
    REQUIRE( jack->getGridLogic()->getLaunchedScene() == s );
  }
  
  
  /// SCENE LAUNCH
  lc->init();
  jack->getGridLogic()->launchScene( s );
  REQUIRE( jack->getGridLogic()->getLaunchedScene() == s );
  
  /// PAD STATE CHECKS
  // empty -> recording
  lc->init();
  REQUIRE( lc->getState() == GridLogic::STATE_EMPTY );
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  REQUIRE( lc->getState() == GridLogic::STATE_RECORD_QUEUED );
  jack->getGridLogic()->bar();
  REQUIRE( lc->getState() == GridLogic::STATE_RECORDING );
  // recording -> playing
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  REQUIRE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
  jack->getGridLogic()->bar();
  REQUIRE( lc->getState() == GridLogic::STATE_PLAYING );
  // playing -> stopped
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  REQUIRE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
  jack->getGridLogic()->bar();
  REQUIRE( lc->getState() == GridLogic::STATE_STOPPED );
  // stopped -> playing
  jack->getGridLogic()->pressed( t, s );
  jack->getGridLogic()->released( t, s );
  REQUIRE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
  jack->getGridLogic()->bar();
  REQUIRE( lc->getState() == GridLogic::STATE_PLAYING );
  
}
#endif

