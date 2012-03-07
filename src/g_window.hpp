
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LUPPP_G_WINDOW
#define LUPPP_G_WINDOW

#include <sstream>

#include <gtkmm.h>

class Top;

#include "g_statestore.hpp"

#include "g_track.hpp"
#include "g_sends.hpp"
#include "g_progress.hpp"

#include "g_audiosource.hpp"
#include "g_trackoutput.hpp"
#include "g_clipselector.hpp"
#include "g_fileselector.hpp"
#include "g_masteroutput.hpp"
#include "g_clipselector.hpp"
#include "g_masterprogress.hpp"
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
    Gtk::MenuItem* menuAddAmPitchshifter;
    
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
    GMasterProgress masterProgress;
    ClipSelector* masterClipSelector;
    
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
    //std::vector<GTrack> trackVector;
    std::vector<Gtk::Widget*> effectVector;
    
    // lists of widgets
    std::vector<GSends*> sendsList;
    std::list<TrackOutput*> trackoutputList;
    std::list<ClipSelector*>clipselectorList;
    
    std::list<Gtk::Label*> tracklabelList;
    std::vector<Gtk::Box*> effectTrackBoxVector;
    std::vector<GAudioSource*> audioSourceVector;
    
    std::list<Gtk::EventBox*> tracklabelBoxList;
    
    std::list<Gtk::ComboBoxText*> trackinputList;
    std::vector<GProgress*> progressWidgetVector;
    
    // iters for lists of widgets
    std::list<ClipSelector*>::iterator clipselectorIter;
    std::list<Gtk::ComboBoxText*>::iterator inputIter;
    
};

#endif
