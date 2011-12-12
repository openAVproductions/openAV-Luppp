
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <sstream>

#include <gtkmm.h>

#include "top.hpp"
#include "g_statestore.hpp"

#include "g_trackoutput.hpp"
#include "g_clipselector.hpp"
#include "trackoutputstate.hpp"


#include "g_lowpass.hpp"
#include "g_equalizer.hpp"

class Window
{
  public:
    Window(Gtk::Main *kit, Top* t);
  
  private:
    Top* top;
    Gtk::Main* kit;
    
    Gtk::Window* window;
    Gtk::Table* mainTable;
    Gtk::HBox* trackEffectBox;
    
    GuiStateStore guiState;
    
    int currentEffectsTrack;
    void setEffectsBox(int trackID);
    
    Equalizer* equalizer;
    GLowPass* lowPass;
    
    // helper functions
    int numTracks;
    void addTrack();
    
    int handleEvent();
    
    
    // lists of widgets
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
