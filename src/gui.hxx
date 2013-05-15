
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "gtrack.hxx"

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
    Avtk::Button*       metronomeButton;
    vector<GTrack*>     tracks;
};

#endif // LUPPP_GUI
