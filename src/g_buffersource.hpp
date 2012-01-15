
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/
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
