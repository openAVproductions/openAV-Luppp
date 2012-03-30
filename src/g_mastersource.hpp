
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

#ifndef LUPPP_GMASTERSOURCE
#define LUPPP_GMASTERSOURCE

class Top;

#include <vector>
#include <iostream>

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

class GMasterSource : public Gtk::DrawingArea
{
  public:
    GMasterSource(Top*);
    
    bool redraw();
    void setPlaying(bool);
  
  protected:
    Top* top;
    
    bool playing;
    
    bool on_expose_event(GdkEventExpose* event);
    //bool on_button_press_event(GdkEventButton* event);
};

#endif // SCENE_SELECTOR
