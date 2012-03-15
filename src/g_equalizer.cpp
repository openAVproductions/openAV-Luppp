
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

#include "g_equalizer.hpp"

#include "g_widgets.hpp"
#include "top.hpp"

using namespace std;
using namespace Luppp;

Equalizer::Equalizer(GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  stateStore = s;
  
  freqState.active = true;
  freqState.type = FREQUENCY_GRAPH_TYPE_PARAMETRIC;
  for(int i = 0; i < 4; i++)
  {
    freqState.Q[i] = 1.0;
    freqState.cutoffFreq[i] = (i/4.f) * 10000;
    freqState.gain[i] = 0.f;
  }
  
  //std::cout << "Enterin Equalizer contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &Equalizer::on_button_press_event) );
  
  set_size_request(75, 37);
}

bool Equalizer::redraw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
      Gdk::Rectangle r(0, 0, get_allocation().get_width(),
              get_allocation().get_height());
      win->invalidate_rect(r, false);
  }
  return true;
}


bool Equalizer::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  if(window)    // Only run if Window does exist
  {
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    
    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();
    
    cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
    setColour(cr, COLOUR_GREY_3 );
    cr->fill();
    
    float low  = stateStore->effectState.at(ID).values[0] * 2 - 1;
    float lmid = stateStore->effectState.at(ID).values[1] * 2 - 1;
    float hmid = stateStore->effectState.at(ID).values[2] * 2 - 1;
    float high = stateStore->effectState.at(ID).values[3] * 2 - 1;
    
    freqState.gain[0] = stateStore->effectState.at(ID).values[0];
    freqState.gain[1] = stateStore->effectState.at(ID).values[1];
    freqState.gain[2] = stateStore->effectState.at(ID).values[2];
    freqState.gain[3] = stateStore->effectState.at(ID).values[3];
    
    int dialSize = 30;
    
    bool active = stateStore->effectState.at(ID).active;
    
    Dial(cr, active, 30 , 150 , low  , DIAL_MODE_PAN);
    Dial(cr, active, 80 , 150 , lmid , DIAL_MODE_PAN);
    Dial(cr, active, 130, 150 , hmid , DIAL_MODE_PAN);
    Dial(cr, active, 180, 150 , high , DIAL_MODE_PAN);
    
    FrequencyGraph(cr, active, 10, 22, 225, 95, freqState );
    
    TitleBar(cr, 0,0 , 250, 216, "Parametric EQ", active);
    
    /*
    if ( state.selected )
    {
      cr->rectangle(0, 0, 74, 216);
      setColour( cr, COLOUR_PURPLE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
    */
    
  }
  return true;
}

void Equalizer::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void Equalizer::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool Equalizer::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    if ( event->button == 3 )
    {
      // doesn't have TOP yet
      /*
      std::cout << "GAmPitchShift Enable / Disable click event!" << std::endl;
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
      top->toEngineQueue.push(x);
      */
      return true;
    }
  }
  else
    return false;
}

Equalizer::~Equalizer()
{
}
