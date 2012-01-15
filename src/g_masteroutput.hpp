
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

#ifndef LUPPP_GMASTEROUTPUT
#define LUPPP_GMASTEROUTPUT

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

#include "top.hpp"
#include "g_statestore.hpp"
#include "trackoutputstate.hpp"

class GMasterOutput : public Gtk::DrawingArea
{
  public:
    GMasterOutput(Top*,GuiStateStore*);
    ~GMasterOutput();
    
    bool redraw();
  
  protected:
    enum ClickedWidget {
      CLICKED_WIDGET_NONE,
      CLICKED_WIDGET_DIAL,
      CLICKED_WIDGET_FADER,
      CLICKED_WIDGET_HEADPHONE_SELECT,
      CLICKED_WIDGET_ROTATE_PAN,
      CLICKED_WIDGET_ROTATE_ELEVATION,
    };
    
    ClickedWidget clickedWidget;
    
    int ID;
    static int privateID;
    
    Top* top;
    GuiStateStore* stateStore;
    
    bool  headphonePflSelect;
    float headphonesVolume;
    float headphonesDialClickOffset;
    float headphonesDialClickVolume;
    
    float volume;
    float pan;
    float elevation;
    
    Glib::RefPtr<Gdk::Pixbuf> headphoneImage;
    
    int mouseX, mouseY;
    
    //Override default signal handler:
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool onMouseMove(GdkEventMotion* event);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
};

#endif // SCENE_SELECTOR
