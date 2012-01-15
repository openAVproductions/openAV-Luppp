
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

#ifndef LUPPP_WAVEVIEW
#define LUPPP_WAVEVIEW

#include <gtkmm/drawingarea.h>
#include <gtkmm.h>

class GWaveView : public Gtk::DrawingArea
{
public:
  GWaveView();
  ~GWaveView();
  
  void redraw();
  
  void setSample(const std::vector<float>& inSample);

protected:
  // the sample "loaded" is the real one. NOT a copy (ie: ByRef)
  std::vector<float> sample;
  
  int waveviewSize;
  
  void setSize(int);
  
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
