
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

#include "g_mastersource.hpp"

#include "top.hpp"
#include "offlineworker.hpp"
#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GMasterSource::GMasterSource(Top* t)
{
  top = t;
  
  playing = true;
  
  add_events(Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GMasterSource::on_button_press_event) );
  
  set_size_request(74*3, 20);
}

bool GMasterSource::redraw()
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

void GMasterSource::setPlaying(bool p )
{
  playing = p;
  redraw();
}

/*
bool GMasterSource::on_button_press_event(GdkEventButton* event)
{
  
  std::cout << "Event Type: " << event->type << ". Event Button: " << event->button << "." << std::endl;
  
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    std::cout << "Event Type: " << event->type << ". Event Button: " << event->button << "." << std::endl;
    
    if(event->button == 3)
    {
      //ifdef DEBUG
      std::cout << "GMasterSource.cpp: Event RightClick on GMasterSource instance at " << this <<"." << std::endl;
      
      pMenu.popup(event->button, event->time);
    }
    else if (event->button == 1)
    {
      // normal click: trigger !currentRecordState
      //lo_send( lo_address_new( NULL,"14688") , "/luppp/track/record","ii",ID, !recording );
    }
    
    return true; //It's been handled.
  }
  else
  {
    std::cout << event -> type << std::endl;
  }
  
  return true;
}
*/

bool GMasterSource::on_expose_event(GdkEventExpose* event)
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
    
    cr->rectangle(event->area.x, event->area.y,
       event->area.width, event->area.height);
    if ( playing )
      setColour(cr, COLOUR_GREEN_1 );
    cr->fill();
    
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    cr->set_font_size ( 13 );
    cr->move_to ( 11 , 15 );
    setColour(cr, COLOUR_ORANGE_1 );
    if ( playing )
      setColour(cr, COLOUR_GREY_3 );
    cr->show_text ( "Master" );
    
  }
  return true;
}
