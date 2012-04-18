
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.hpp"

#include "top.hpp"

// for reading sample pack info stuffs
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

Settings::Settings(Top* t)
{
  top = t;
  
  
  showTooltips = true;
  
  // here we attempt to load .luppprc from the users home dir
  std::string userHome = Glib::get_home_dir();
  
  std::string lupppRcFilename = Glib::build_filename(userHome, ".luppprc");
  
  bool fileExists = Glib::file_test ( lupppRcFilename , Glib::FILE_TEST_EXISTS );
  if ( fileExists )
  {
    cout << "Found Luppprc file, parsing now!" << endl;
    Config config;
    
    try
    {
      config.readFile ( lupppRcFilename.c_str() );
    }
    catch(const ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
    }
    
    try
    {
      config.lookupValue("luppp.settings.showtooltips", showTooltips );
      
      cout << "Show tooltips: " << showTooltips << endl;
      
      //if( config.lookupValue( "luppp.samplePack.s"+ toString(i) +".numBeats" , numBeats) &&
      //config.lookupValue( "luppp.samplePack.s"+ toString(i) +".name"     , sampleName) )
    }
    catch ( ParseException &e )
    {
      // some libconfig error, like ParseException, or SettingException
      cout << "Settings::loadAudioBuffer() LibConfig Parsing exception... ignoring" << endl; 
    }
  }
  else
  {
    cout << "Could not find a '.luppprc' file in user home dir!" << endl;
  }
  
}




