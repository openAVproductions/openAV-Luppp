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

class GBufferSource : public Gtk::DrawingArea
{
public:
  GBufferSource(Top* t, GuiStateStore* s);
  ~GBufferSource();
  
  bool redraw();
  

protected:
  int ID;
  static int privateID;
  Top* top;
  GuiStateStore* stateStore;
  
  // popup menu stuff:
  Gtk::Menu    pMenu;
  Gtk::MenuItem  setLv2Source;
  Gtk::MenuItem  setFluidSynthSource;
  Gtk::MenuItem  setWhiteNoiseSource;
  
  void fluidSynthSource();
  void whiteNoiseSource();
  
  // Button & Expose event.
  bool on_button_press_event(GdkEventButton* event);
  bool on_expose_event(GdkEventExpose* event);
};

#endif
