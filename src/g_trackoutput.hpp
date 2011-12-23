#ifndef LUPPP_TRACKOUTPUT
#define LUPPP_TRACKOUTPUT

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

#include "top.hpp"
#include "g_statestore.hpp"
#include "trackoutputstate.hpp"

class TrackOutput : public Gtk::DrawingArea
{
  public:
    TrackOutput(Top*,GuiStateStore*);
    ~TrackOutput();
    
    bool redraw();
  
  protected:
    enum ClickedWidget {
      CLICKED_WIDGET_NONE,
      CLICKED_WIDGET_DIAL,
      CLICKED_WIDGET_FADER,
    };
    
    ClickedWidget clickedWidget;
    
    int ID;
    static int privateID;
    GuiStateStore* stateStore;
    
    Top* top;
    
    int mouseX, mouseY;
    
    //Override default signal handler:
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool onMouseMove(GdkEventMotion* event);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
};

#endif // SCENE_SELECTOR
