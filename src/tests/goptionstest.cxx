

#include "../goptions.hxx"

#ifdef BUILD_TESTS

#include "../config.hxx"
#include "qunit.hxx"

int GOptions::runTests()
{
  QUnit::UnitTest qunit( QUnit::normal, true );
  
  OptionsWindow opts;
  opts->show();
  
  Fl::run();
  
  //QUNIT_IS_TRUE( jack->getGridLogic()->getLaunchedScene() == s );
  
}

#endif // BUILD_TESTS

