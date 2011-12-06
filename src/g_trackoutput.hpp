#ifndef LUPPP_TRACKOUTPUT
#define LUPPP_TRACKOUTPUT

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

#include "g_statestore.hpp"

#include "trackoutputstate.hpp"

class TrackOutput : public Gtk::DrawingArea
{
  public:
    TrackOutput(GuiStateStore*);
    ~TrackOutput();
    
    bool redraw();
  
  protected:
    int ID;
    static int privateID;
    GuiStateStore* stateStore;
    
    
    //Override default signal handler:
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
};

#endif // SCENE_SELECTOR
