
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
