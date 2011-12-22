#ifndef LUPPP_TRANSIENT
#define LUPPP_TRANSIENT

#include <vector>
#include <iostream>

class Top;
class GuiStateStore;

#include "g_widgetbase.hpp"
#include <gtkmm/drawingarea.h>

class GTransient : public Gtk::DrawingArea, public WidgetBase
{
  public:
    GTransient(Top*, GuiStateStore*);
    ~GTransient();
    
    bool redraw();
  
  protected:
    int ID;
    GuiStateStore* stateStore;
    
    Top* top;
    
    float cutoff;
    float q;
    int xSize, ySize;
    
    bool mouseDown;
    
    
    //Override default signal handler:
    bool onMouseMove(GdkEventMotion* event);
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
};

#endif // SCENE_SELECTOR
