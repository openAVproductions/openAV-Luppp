

#ifdef BUILD_TESTS

#include "../config.hxx"

#include "../gui.hxx"
#include "../audiobuffer.hxx"
#include "../diskreader.hxx"

#include "qunit.hxx"

extern Gui* gui;
extern bool testsPassed;

int DiskReader::runTests()
{
  QUnit::UnitTest qunit( QUnit::normal );
  
  // set the session path to /tmp for test writing
  
  string path = "/tmp";
  string session = "testSession";
  
  //AudioBuffer ab(440);
  //gui->getDiskWriter()->initialize(path, session);
  
  QUNIT_IS_TRUE( gui->getDiskReader()->loadSample( 0, 0,"/tmp/lupppTestMaterial/beat.wav" ) == LUPPP_RETURN_OK );
  
  QUNIT_IS_TRUE( gui->getDiskReader()->readSession("/tmp/lupppTestMaterial/lupppTest" ) == LUPPP_RETURN_OK );
  
  return qunit.errors();
}

#endif

