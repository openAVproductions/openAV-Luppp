
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "avtk/avtk_light_button.h"

#include "config.hxx"
#include "gtrack.hxx"
#include "gmastertrack.hxx"

#include <vector>

using namespace std;

class Gui
{
  public:
    Gui();
    int show();
  
  private:
    Fl_Double_Window    window;
    Fl_Box*             box;
    
    GMasterTrack*       master;
    
    vector<GTrack*>     tracks;
};

#endif // LUPPP_GUI
