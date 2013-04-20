
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "gtrack.hxx"

class Gui
{
  public:
    Gui();
    int show();
  
  private:
    Fl_Window*  window;
    Fl_Box*     box;
    GTrack*     track;
    GTrack*     track2;
};

#endif // LUPPP_GUI
