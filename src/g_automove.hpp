
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

#ifndef LUPPP_AUTOMOVE
#define LUPPP_AUTOMOVE

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include "g_widgets.hpp"

#include "automove.hpp"

using namespace Luppp;

class Top;

class GAutoMove : public Gtk::DrawingArea
{
  public:
    GAutoMove(Top*);
    
    void redraw();
    
    void setType(int);
    void setDuration(int);
    void setProgress(float);

  protected:
    Top* top;
    
    int width;
    int height;
    
    float progress;
    AutoMoveType type;
    AutoMoveDuration duration;
    
    // helper draw functions
    void drawNoBuffer(Cairo::RefPtr<Cairo::Context>);
    
    // marker drawing function
    //void drawMarker(Cairo::RefPtr<Cairo::Context> cr, float position, Colour col, MarkerType type, std::string& name);
    
    // popup menu stuff:
    Gtk::Menu      pMenu;
    Gtk::MenuItem  setSize1Sec;
    Gtk::MenuItem  setSizeHalfSec;
    Gtk::MenuItem  setSizeQuaterSec;
    Gtk::MenuItem  setSizeEigthSec;
    
    // Button & Expose event.
    bool on_button_press_event(GdkEventButton* event);
    bool on_expose_event(GdkEventExpose* event);
};

#endif
