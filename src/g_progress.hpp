#ifndef LUPPP_GPROGRESS
#define LUPPP_GPROGRESS

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>


class GProgress : public Gtk::DrawingArea
{
  public:
    GProgress();
    
    bool redraw();
    void setValue(float);
  
  protected:
    float value;
    
    bool on_expose_event(GdkEventExpose* event);
};

#endif // SCENE_SELECTOR
