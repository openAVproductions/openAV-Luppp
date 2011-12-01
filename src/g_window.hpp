
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <gtkmm.h>

#include "top.hpp"
#include "statestore.hpp"

class Window : public Gtk::Window
{
  public:
    Window(Top* t);
  
  private:
    Top* top;
    
    StateStore guiState;
};

#endif
