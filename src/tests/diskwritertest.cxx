

#ifdef BUILD_TESTS

#include "../config.hxx"

#include "../gui.hxx"
#include "../audiobuffer.hxx"
#include "../diskwriter.hxx"

#include "qunit.hxx"

extern Gui* gui;

int DiskWriter::runTests()
{
  QUnit::UnitTest qunit( QUnit::normal );
  //AudioBuffer ab;
  //QUNIT_IS_TRUE( gui->getDiskWriter()->writeAudioBuffer(0, 0, &ab) == LUPPP_RETURN_OK );
  
  QUNIT_IS_TRUE( gui->getDiskWriter()->writeSession("/tmp","luppTestSession") == LUPPP_RETURN_OK );
  
  return 0;
}

#endif

