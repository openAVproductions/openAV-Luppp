

#ifdef BUILD_TESTS

#include "../config.hxx"

#include "../gui.hxx"
#include "../audiobuffer.hxx"
#include "../diskwriter.hxx"

#include "../controller/nonseq.hxx"
#include "../controller/genericmidi.hxx"

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
  
  /**  write controller test:
   *  This test ensures that when writing a GenericMIDI* controller to disk the
   *  data is correct. A dummy controller instance is setup, and then written to
   *  disk.
  **/
  int waste = 0;
  GenericMIDI* dummy = new GenericMIDI( waste, "dummy" );
  std::string name = "dummy name";
  std::string author = "dummy author";
  std::string link = "www.dummylink.com";
  
  int d = gui->getDiskWriter()->writeControllerFile( name, author, link, dummy );
  QUNIT_IS_TRUE( d == LUPPP_RETURN_OK );
  
  dummy->setupBinding( Event::TRACK_VOLUME, 176, 7, 0, 0, 0, 0 );
  dummy->setupBinding( Event::GRID_LAUNCH_SCENE, 144, 60, 0, 2, 0, 0 );
  
  int d1 = gui->getDiskWriter()->writeControllerFile( name, author, link, dummy );
  QUNIT_IS_TRUE( d1 == LUPPP_RETURN_OK );
  
  /// test dynamic cast, null, and invalid Controller* type
  int r1 = gui->getDiskWriter()->writeControllerFile(name, author, link, 0);
  QUNIT_IS_TRUE( r1 == LUPPP_RETURN_ERROR );
  
  Controller* non = new NonSeq();
  int r2 = gui->getDiskWriter()->writeControllerFile(name, author, link, non);
  QUNIT_IS_TRUE( r2 == LUPPP_RETURN_ERROR );
  
  return qunit.errors();
}

#endif

