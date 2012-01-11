
#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <sstream>

#include <gtkmm.h>

class Top;

#include "g_statestore.hpp"

#include "g_track.hpp"
#include "g_progress.hpp"

#include "g_trackoutput.hpp"
#include "g_fileselector.hpp"
#include "g_masteroutput.hpp"
#include "g_clipselector.hpp"
#include "g_effectselector.hpp"
#include "trackoutputstate.hpp"
#include "g_instrumentselector.hpp"

#include "g_waveview.hpp"

class Window
{
  public:
    Window(Gtk::Main *kit, Top* t);
  
  private:
    Top* top;
    Gtk::Main* kit;
    
    Gtk::HBox* mainBox;
    Gtk::Label* eeLabel;
    Gtk::Window* window;
    Gtk::Table* mainTable;
    Gtk::Box* masterOutputBox;
    
    Gtk::HBox* trackEffectBox;
    Gtk::EventBox* trackEffectEventBox;
    
    Gtk::Viewport* fileChooserViewport;
    
    Gtk::Box* effectChooserBox;
    Gtk::Box* instrumentChooserBox;
    
    Gtk::Paned* fileChooserPane;
    Gtk::ToggleButton* fileBrowserToggle;
    
    Gtk::MenuItem* menuFileAddTrack;
    Gtk::MenuItem* menuFileQuit;
    
    Gtk::MenuItem* menuAddReverb;
    Gtk::MenuItem* menuAddLowpass;
    Gtk::MenuItem* menuAddLimiter;
    Gtk::MenuItem* menuAddHighpass;
    Gtk::MenuItem* menuAddTransient;
    Gtk::MenuItem* menuAddCompressor;
    Gtk::MenuItem* menuAddParametric;
    Gtk::MenuItem* menuAddTrancegate;
    Gtk::MenuItem* menuAddBeatsmasher;
    
    GuiStateStore guiState;
    
    int currentEffectsTrack;
    int previousEffectsTrack;
    void setEffectsBox(int trackID);
    
    FileSelector fileSelector;
    EffectSelector effectSelector;
    InstrumentSelector instrumentSelector;
    
    GWaveView waveview;
    GWaveView inputWaveview;
    
    GMasterOutput masterOutput;
    
    // helper functions
    void redrawEffectBox();
    void setFileChooserPane();
    void trackEffectDragDrop(const Glib::RefPtr<Gdk::DragContext>& context, int, int,
                             const Gtk::SelectionData& selection_data, guint, guint time);
    
    void sendAddTrack();
    
    void quit();
    
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
    std::vector<GProgress*> progressWidgetVector;
    
    // iters for lists of widgets
    std::list<ClipSelector*>::iterator clipselectorIter;
    std::list<Gtk::ComboBoxText*>::iterator inputIter;
    
};

#endif
