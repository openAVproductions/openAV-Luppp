
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include "config.hxx"
#include "gtrack.hxx"
#include "gunittrack.hxx"
#include "gmastertrack.hxx"
#include "gaudioeditor.hxx"

#include "diskwriter.hxx"
#include "diskreader.hxx"

#include <vector>
#include <string>

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
    
    /// shows the options window
    void showOptions();
    
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
  
  private:
    Fl_Double_Window    window;
    // FIXME: Refactor optionWindow to seperate class
    Fl_Double_Window* optionWindow;
    
    AudioEditor*        audioEditor;
    
    DiskReader*         diskReader;
    DiskWriter*         diskWriter;
    
    GMasterTrack*       master;
    
    vector<GTrack*>     tracks;
    
    // FIXME: refactor tooltip code out..?
    std::string         tooltip;
    Fl_Box*             tooltipLabel;
};

#endif // LUPPP_GUI
