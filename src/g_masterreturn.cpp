
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

#include "g_masterreturn.hpp"

#include "settings.hpp"
#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

int GMasterReturn::privateID = 0;

GMasterReturn::GMasterReturn(Top* t, GuiStateStore* s)
{
  ID = privateID++;
  
  top = t;
  stateStore = s;
  
  mouseX = -1;
  mouseY = -1;
  
  value = 0.f;
  
  //std::cout << "Enterin GMasterReturn contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GMasterReturn::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GMasterReturn::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GMasterReturn::onMouseMove ) );
  
  if ( top->settings->showTooltips )
    set_tooltip_text ( "JACK returns: Amount to mix into master bus" );
  
  set_size_request(72,39);
}

bool GMasterReturn::redraw(float newVal)
{
  value = newVal;
  
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


bool GMasterReturn::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  if(window)    // Only run if Window does exist
  {
    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();
    
    cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
    cr->set_source_rgb(0.1 , 0.1 , 0.1 );
    cr->fill();
    
    
    Dial(cr, true, 35 , 0, value, DIAL_MODE_SEND ); // fader
    
    // Dial text "A"
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    cr->set_font_size ( 20 );
    cr->move_to ( 10, 28);
    switch ( ID )
    {
      case 0: setColour(cr, COLOUR_BLUE_1 ); cr->show_text ( "A" ); break;
      case 1: setColour(cr, COLOUR_BLUE_1 ); cr->show_text ( "B" ); break;
      case 2: setColour(cr, COLOUR_BLUE_1 ); cr->show_text ( "C" ); break;
    }
    
  }
  return true;
}

void GMasterReturn::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GMasterReturn::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GMasterReturn::on_button_press_event(GdkEventButton* event)
{
  clickedWidget = CLICKED_WIDGET_NONE;
  
  if( event->type == GDK_BUTTON_PRESS && event->button == 1 )
  {
    // reset click, will be set again if needed
    clickedWidget = CLICKED_WIDGET_DIAL;
    
    mouseY = event->y;
  }
  
  return true;
}
bool GMasterReturn::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE && event->button == 1 )
  {
    mouseX = -1;
    mouseY = -1;
  }
  
  clickedWidget = CLICKED_WIDGET_NONE;
  
  return true;
}

bool GMasterReturn::onMouseMove(GdkEventMotion* event)
{
  if ( clickedWidget == CLICKED_WIDGET_DIAL )
  {
    float mouseYdelta = (mouseY - event->y) / 25.f;
    std::cout << "GMaster Return: MouseYdelta: " << mouseYdelta << std::endl;
    
    float newVal = value + mouseYdelta;
    if ( newVal > 1 ) newVal  = 1.f;
    if ( newVal < 0 ) newVal  = 0.f;
    
    EngineEvent* x = new EngineEvent();
    // move volume relative to current value
    x->setMixerReturnVolume(ID, newVal );
    top->toEngineQueue.push(x);
    
    //redraw( newVal );
    
    // reset mouseY
    mouseY = event->y;
  }
  
  return true;
  
}

GMasterReturn::~GMasterReturn()
{
}
