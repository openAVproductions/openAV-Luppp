
/*
  Copyright 2012, Harry van Haaren
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <signal.h>

#include "top.hpp"
#include "jackclient.hpp"

#include <gtkmm.h>
#include "g_window.hpp"

using namespace std;

static Top* top = 0;
static JackClient* jackClient = 0;

void sighandler(int)
{
  jackClient->deactivate();
  exit(0);
}

int main(int argc, char** argv)
{
  cout << "Luppp 2.0" << endl;
  
  Gtk::Main kit (argc, argv);
  
  signal(SIGINT, &sighandler);
  
  top = new Top();
  jackClient = new JackClient(top);
  top->setJackPointer( jackClient );
  
  // we pass the GUI the Top* too, its the method of interaction between
  // engine and GUI. We also pass in "kit", and then we run the mainloop
  Window window( &kit, top);
  
  return 0;
}
