
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
    Gui();
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
    
    GTrack* getTrack(int id);
    GMasterTrack* getMasterTrack(){return master;}
    
    DiskWriter* getDiskWriter(){return diskWriter;}
    DiskReader* getDiskReader(){return diskReader;}
    
    /// used to load samples into the grid
    void selectLoadSample( int track, int clip );
    
    /// allows the user to select a Controller definition
    static void selectLoadController(Fl_Widget* w, void*);
    
    
    int samplerate;
    
    nsm_client_t* getNsm(){return nsm;}
  
  private:
    Fl_Double_Window    window;
    
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
