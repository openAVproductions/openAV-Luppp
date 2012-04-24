
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

#include <glib.h>
#include <glib/gstdio.h>

// for reading sample pack info stuffs
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

Settings::Settings(Top* t)
{
  top = t;
  
  
  showTooltips = true;
  
  // here we attempt to load .luppprc from the users home dir
  userHome = Glib::get_home_dir();
  
  dotLupppDir = Glib::build_filename(userHome, ".luppp");
  lupppRcFilename = Glib::build_filename( dotLupppDir, "luppprc");
  
  bool dirExists = Glib::file_test ( dotLupppDir , Glib::FILE_TEST_IS_DIR );
  if ( !dirExists )
  {
    cout << ".luppp directory doesn't exist!" << endl;
    g_mkdir( dotLupppDir.c_str(), 0755);
  }
  
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
    }
    catch ( ParseException &e )
    {
      cout << "luppp.settings.showtooltips: Error! Key not found" << endl; 
    }
  }
  else
  {
    cout << "Could not find 'luppprc' in settings folder " + dotLupppDir << endl;
    writeDefaults();
  }
}


void Settings::writeDefaults()
{
  cout << "Settings: writeDefaults()" << endl;
  
  // create config, and then read it's root
  Config config;
  Setting &root = config.getRoot();
  
  if(!root.exists("luppp"))
    root.add("luppp", Setting::TypeGroup);
  
  Setting &luppp = root["luppp"];
  
  if( !luppp.exists("settings"))
    luppp.add("settings", Setting::TypeGroup);
  
  Setting &settings = luppp["settings"];
  
  // here we add the "settings", and thier default values
  settings.add("showtooltips",Setting::TypeInt) = 1;
  
  
  // Write out the updated configuration.
  try
  {
    config.writeFile( lupppRcFilename.c_str() );
  }
  catch(const FileIOException &fioex)
  {
    cerr << "I/O error while writing file: " << lupppRcFilename.c_str() << endl;
  }
  
  
  
}
