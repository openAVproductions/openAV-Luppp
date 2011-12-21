
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <sstream>

#include <gtkmm.h>

#include "top.hpp"

#include "g_statestore.hpp"

#include "g_track.hpp"

#include "g_trackoutput.hpp"
#include "g_clipselector.hpp"
#include "trackoutputstate.hpp"


#include "g_lowpass.hpp"
#include "g_highpass.hpp"
#include "g_waveview.hpp"
#include "g_equalizer.hpp"
#include "g_compressor.hpp"

class Window
{
  public:
    Window(Gtk::Main *kit, Top* t);
  
  private:
    Top* top;
    Gtk::Main* kit;
    
    Gtk::HBox* mainBox;
    Gtk::Window* window;
    Gtk::Table* mainTable;
    Gtk::HBox* trackEffectBox;
    
    Gtk::MenuItem* menuAddLowpass;
    Gtk::MenuItem* menuAddHighpass;
    Gtk::MenuItem* menuAddParametric;
    Gtk::MenuItem* menuAddBeatsmasher;
    Gtk::MenuItem* menuAddReverb;
    Gtk::MenuItem* menuAddTransient;
    Gtk::MenuItem* menuAddCompressor;
    
    GuiStateStore guiState;
    
    int currentEffectsTrack;
    int previousEffectsTrack;
    void setEffectsBox(int trackID);
    
    GWaveView waveview;
    GWaveView inputWaveview;
    
    // helper functions
    void redrawEffectBox();
    
    int numTracks;
    void addTrack();
    
    int handleEvent();
    void addEffect(EffectType);
    
    // holder for other elements
    std::vector<GTrack> trackVector;
    
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
