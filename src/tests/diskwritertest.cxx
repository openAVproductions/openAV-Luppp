

#ifdef BUILD_TESTS

#include "../config.hxx"

#include "../gui.hxx"
#include "../audiobuffer.hxx"
#include "../diskwriter.hxx"

#include "qunit.hxx"

extern Gui* gui;
extern bool testsPassed;

int DiskWriter::runTests()
{
  QUnit::UnitTest qunit( QUnit::normal );
  
  // set the session path to /tmp for test writing
  
  string path = "/tmp";
  string session = "testSession";
  
  AudioBuffer ab(440);
  gui->getDiskWriter()->initialize(path, session);
  
  QUNIT_IS_TRUE( gui->getDiskWriter()->writeAudioBuffer(0, 0, &ab) == LUPPP_RETURN_OK );
  QUNIT_IS_TRUE( gui->getDiskWriter()->writeSession() == LUPPP_RETURN_OK );
  
  QUNIT_IS_TRUE( strcmp( gui->getDiskWriter()->getLastSavePath().c_str(), path.c_str() ) == 0 );
  QUNIT_IS_TRUE( strcmp( gui->getDiskWriter()->getLastSaveName().c_str(), session.c_str() ) == 0 );
  
  return qunit.errors();
}

#endif

