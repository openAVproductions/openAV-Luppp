/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <vector>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include "goptions.hxx"

#include "config.hxx"
#include "gtrack.hxx"
#include "gunittrack.hxx"
#include "gmastertrack.hxx"
#include "gaudioeditor.hxx"

#include "diskwriter.hxx"
#include "diskreader.hxx"

// non-session-manager integration
#include "nsm.h"


using namespace std;

class AudioBuffer;

class Gui
{
  public:
    Gui(const char* argZero);
    ~Gui();
    
    int show();
    
    int quit();
    void askQuit();
    
    /// returns the options window
    OptionsWindow* getOptionsWindow();
    
    /// open audio editor window with an AudioBuffer
    AudioEditor* getAudioEditor();
    
    /// resets the state to "new"
    void reset();
    
    /// sets up MIDI controllers: must be done *after* backend is started
    void addMidiControllerToSetup(std::string);
    void setupMidiControllers();
    
    GTrack* getTrack(int id);
    GMasterTrack* getMasterTrack(){return master;}
    
    DiskWriter* getDiskWriter(){return diskWriter;}
    DiskReader* getDiskReader(){return diskReader;}
    
    /// used to load samples into the grid
    void selectLoadSample( int track, int clip );
    
    /// allows the user to select a Controller definition
    static void selectLoadController(Fl_Widget* w, void*);
    
    
    int samplerate;
    
    int getWindowWidth(){return window.w();}
    
    nsm_client_t* getNsm(){return nsm;}
  
  private:
    vector<std::string> controllerVector;
    
    Fl_Double_Window    window;
    
    Fl_Group* lupppGroup; 
    
    OptionsWindow*    optionWindow;
    
    AudioEditor*        audioEditor;
    
    DiskReader*         diskReader;
    DiskWriter*         diskWriter;
    
    GMasterTrack*       master;
    
    vector<GTrack*>     tracks;
    
    // FIXME: refactor tooltip code out..?
    std::string         tooltip;
    Fl_Box*             tooltipLabel;
    
    // non-session-manager
    nsm_client_t* nsm;
};

#endif // LUPPP_GUI
