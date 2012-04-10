
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

#ifndef LUPPP_GSCOPE
#define LUPPP_GSCOPE

#include <gtkmm/drawingarea.h>
#include <gtkmm.h>

#include "g_widgets.hpp"

using namespace Luppp;

class Top;

class GScope : public Gtk::DrawingArea
{
  public:
    GScope(Top*);
    ~GScope();
    
    void redraw();
    
    void setSample(const std::vector<float>& inSample);

  protected:
    Top* top;
    
    enum MarkerType {
      MARKER_START = 0,
      MARKER_MIDDLE,
      MARKER_END,
    };
    
    int width;
    int height;
    
    // new data copied into this sample, this one is drawn
    std::vector<float> sample;
    
    // zoom stuffs
    int waveviewSize;
    void setSize(int);
    void setSampleID(int);
    
    // marker drawing function
    //void drawMarker(Cairo::RefPtr<Cairo::Context> cr, float position, Colour col, MarkerType type, std::string& name);
    
    // drop function
    //void dropFunction(const Glib::RefPtr<Gdk::DragContext>& context, int, int,
    //                  const Gtk::SelectionData& selection_data, guint, guint time);
    
    // selects which buffer ID we want to edit
    int sampleID;
    
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

#endif // GTKMM_EXAMPLE_WaveView_H
