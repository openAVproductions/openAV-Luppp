
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <gtkmm.h>

#include "top.hpp"
#include "statestore.hpp"

#include "g_trackoutput.hpp"
#include "trackoutputstate.hpp"

class Window
{
  public:
    Window(Gtk::Main *kit, Top* t);
  
  private:
    Top* top;
    Gtk::Main* kit;
    
    Gtk::Window* window;
    Gtk::Table* mainTable;
    
    StateStore guiState;
    
    // helper functions
    void addTrack();
    
    
    // lists of widget
    std::list<TrackOutput*> trackoutputList;
    
};

#endif
