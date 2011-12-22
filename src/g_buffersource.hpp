#ifndef LUPPP_GBUFFERSOURCE
#define LUPPP_GBUFFERSOURCE

#include <gtkmm/drawingarea.h>
#include <gtkmm.h>

#include <iostream>
#include <cairomm/context.h>
#include <sstream>

#include "top.hpp"
#include "g_statestore.hpp"

#include "g_widgets.hpp"
#include "g_widgetbase.hpp"

class GBufferSource : public Gtk::DrawingArea, public WidgetBase
{
public:
  GBufferSource(Top* t, GuiStateStore* s);
  ~GBufferSource();
  
  bool redraw();
  

protected:
  int ID;
  Top* top;
  GuiStateStore* stateStore;
  
  // popup menu stuff:
  Gtk::Menu    pMenu;
  Gtk::MenuItem  menuRetrigger;
  Gtk::MenuItem  menuLoadSample;
  Gtk::MenuItem  menuWriteSample;
  Gtk::MenuItem  menuSeparator;
  Gtk::MenuItem  menuBeats2;
  Gtk::MenuItem  menuBeats4;
  Gtk::MenuItem  menuBeats8;
  Gtk::MenuItem  menuBeats12;
  Gtk::MenuItem  menuBeats16;
  Gtk::MenuItem  menuBeats32;
  
  // Button & Expose event.
  bool on_button_press_event(GdkEventButton* event);
  bool on_expose_event(GdkEventExpose* event);
};

#endif
