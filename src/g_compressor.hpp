#ifndef LUPPP_GCOMPRESSOR
#define LUPPP_GCOMPRESSOR

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

#include "top.hpp"
#include "g_statestore.hpp"

#include "g_widgetbase.hpp"

// inherit from WidgetBase so that we can define the ID of the widget,
// which tell us which indice of the EffectStateVector to use to redraw
class GCompressor : public Gtk::DrawingArea, public WidgetBase
{
  public:
    GCompressor(Top*, GuiStateStore*);
    ~GCompressor();
    
    bool redraw();
  
  protected:
    int ID;
    static int privateID;
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
