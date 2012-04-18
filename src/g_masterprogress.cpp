
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

#include "g_masterprogress.hpp"

#include "top.hpp"
#include <sstream>
#include "settings.hpp"
#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GMasterProgress::GMasterProgress(Top* t)
{
  top = t;
  
  set_size_request(74, 18);
  bpm = 160;
  
  if ( top->settings->showTooltips )
    set_tooltip_text ( "Master Progress: Shows downbeats in Luppp's internal engine" );
  
}

bool GMasterProgress::redraw()
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

void GMasterProgress::setProgress(float p)
{
  progress = p;
  redraw();
}

void GMasterProgress::setBpm(int inBpm)
{
  bpm = inBpm;
  redraw();
}

bool GMasterProgress::on_expose_event(GdkEventExpose* event)
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
    
    int stage = (int)(progress * 4);
    
    switch (stage)
    {
      case 0:
      case 1: { setColour(cr, COLOUR_GREEN_1 ); break; }
      case 2: { setColour(cr, COLOUR_ORANGE_1 ); break; }
      case 3: { setColour(cr, COLOUR_RECORD_RED ); break; }
    }
    
    int quaterWidth = event->area.width/4.f;
    
    cr->rectangle( quaterWidth * stage, event->area.y,
                   quaterWidth, event->area.height);
    cr->fill();
    
    
    int x = 2;
    int y = 2;
    
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    cr->set_font_size ( 15 );
    cr->move_to ( x + 20, (y + 10) );
    setColour(cr, COLOUR_GREY_4);
    
    stringstream s;
    s << bpm;
    cr->show_text ( s.str() );
    
  }
  return true;
}
