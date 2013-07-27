
#ifndef LUPPP_GUI
#define LUPPP_GUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "config.hxx"
#include "gtrack.hxx"
#include "gunittrack.hxx"
#include "gmastertrack.hxx"

#include <vector>
#include <string>

using namespace std;

class Gui
{
  public:
    Gui();
    int show();
    GTrack* getTrack(int id);
    GMasterTrack* getMasterTrack(){return master;}
    
    // for pushing strings to tooltip area
    void setTooltip( std::string s );
  
  private:
    Fl_Double_Window    window;
    Fl_Box*             box;
    
    GMasterTrack*       master;
    
    vector<GTrack*>     tracks;
    
    std::string         tooltip;
    Fl_Box*             tooltipLabel;
};

#endif // LUPPP_GUI
