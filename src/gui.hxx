
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

class Gui
{
  public:
    Gui();
    int show();
  
  private:
    Fl_Window*  window;
    Fl_Box*     box;
};

#endif // LUPPP_GUI
