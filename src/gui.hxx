
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "config.hxx"
#include "gtrack.hxx"
#include "gunittrack.hxx"
#include "gmastertrack.hxx"

#include "diskwriter.hxx"
#include "diskreader.hxx"

#include <vector>
#include <string>

using namespace std;

class Gui
{
  public:
    Gui();
    int show();
    
    int quit();
    bool askQuit();
    
    GTrack* getTrack(int id);
    GMasterTrack* getMasterTrack(){return master;}
    
    DiskWriter* getDiskWriter(){return diskWriter;}
    DiskReader* getDiskReader(){return diskReader;}
    
    void selectLoadSample( int track, int clip );
  
  private:
    Fl_Double_Window    window;
    Fl_Box*             box;
    
    DiskReader*         diskReader;
    DiskWriter*         diskWriter;
    
    GMasterTrack*       master;
    
    vector<GTrack*>     tracks;
    
    std::string         tooltip;
    Fl_Box*             tooltipLabel;
};

#endif // LUPPP_GUI
