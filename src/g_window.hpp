
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <sstream>

#include <gtkmm.h>

#include "top.hpp"
#include "statestore.hpp"

#include "g_trackoutput.hpp"
#include "g_clipselector.hpp"
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
    int numTracks;
    void addTrack();
    
    int handleEvent();
    
    
    // lists of widget
    
    std::list<TrackOutput*> trackoutputList;
    std::list<ClipSelector*>clipselectorList;
    
    std::list<Gtk::Label*> tracklabelList;
    std::list<Gtk::ComboBoxText*> trackinputList;
    std::list<Gtk::ProgressBar*> trackprogressList;
    
    // iters for lists of widgets
    std::list<ClipSelector*>::iterator clipselectorIter;
    std::list<Gtk::ComboBoxText*>::iterator inputIter;
    std::list<Gtk::ProgressBar*>::iterator progressIter;
    
};

#endif
