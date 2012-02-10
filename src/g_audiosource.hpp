
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

#ifndef LUPPP_GAUDIOSOURCE
#define LUPPP_GAUDIOSOURCE

class Top;

#include <vector>
#include <iostream>

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

class GAudioSource : public Gtk::DrawingArea
{
  public:
    GAudioSource(Top*);
    
    bool redraw();
    void setPlaying(bool);
  
  protected:
    Top* top;
    
    int ID;
    static int privateID;
    
    bool playing;
    
    // popup menu stuff:
    Gtk::Menu    pMenu;
    Gtk::MenuItem  setLv2Source;
    Gtk::MenuItem  setFluidSynthSource;
    Gtk::MenuItem  setWhiteNoiseSource;
    
    void fluidSynthSource();
    void whiteNoiseSource();
    
    bool on_expose_event(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
};

#endif // SCENE_SELECTOR
