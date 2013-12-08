/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


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

