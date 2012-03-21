
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
#include "g_automove.hpp"

#include <iostream>
#include <cairomm/context.h>
#include <sstream>

#include "g_widgets.hpp"

#include "top.hpp"
#include "offlineworker.hpp"
#include "g_statestore.hpp"

using namespace std;
using namespace Luppp; // for COLOUR_*

GAutoMove::GAutoMove(Top* t)
{
  top = t;
  
  // Gives Exposure & Button presses to the widget.
  add_events(Gdk::EXPOSURE_MASK);
  add_events(Gdk::BUTTON_PRESS_MASK );
  
  progress = 0;
  
  /*
  // add labels to menu, attach them to the right function, and add to menu
  setSize1Sec.set_label("1 second");
  setSizeHalfSec.set_label("1/2 second");
  setSizeQuaterSec.set_label("1/4 second");
  setSizeEigthSec.set_label("1/8 second");
  
  setSize1Sec.signal_activate().connect     ( sigc::bind (sigc::mem_fun(*this, &GAutoMove::setSampleID) , 0 ) );
  setSizeHalfSec.signal_activate().connect  ( sigc::bind (sigc::mem_fun(*this, &GAutoMove::setSampleID) , 1 ) );
  setSizeQuaterSec.signal_activate().connect( sigc::bind (sigc::mem_fun(*this, &GAutoMove::setSampleID) , 2 ) );
  setSizeEigthSec.signal_activate().connect ( sigc::bind (sigc::mem_fun(*this, &GAutoMove::setSampleID) , 3 ) );
  
  pMenu.add( setSize1Sec );
  pMenu.add( setSizeHalfSec );
  pMenu.add( setSizeQuaterSec );
  pMenu.add( setSizeEigthSec );
  pMenu.show_all();
  */
  
  
  // drop destination
  /*
  std::vector<Gtk::TargetEntry> dropTargets;
  dropTargets.push_back( Gtk::TargetEntry("AUDIO_BUFFER_ID") );
  
  drag_dest_set(dropTargets);
  
  signal_drag_data_received().connect(sigc::mem_fun(*this, &GAutoMove::dropFunction) );
  */
  
  // set default widget size
  set_size_request(230, 130);
}

void GAutoMove::setType(int type)
{
  type = static_cast<AutoMoveType>(type);
  redraw();
}

void GAutoMove::setProgress(float p)
{
  progress = p;
  redraw();
}

bool GAutoMove::on_expose_event(GdkEventExpose* event)
{
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    
    if(window)    // Only run if Window does exist
    {
      Gtk::Allocation allocation = get_allocation();
      width = allocation.get_width();
      height = allocation.get_height();
      
      // coordinates for the center of the window
      int xc, yc;
      xc = width / 2;
      yc = height / 2;
      
      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,event->area.width, event->area.height);
      cr->clip_preserve();
      
      // Draw outline shape
      setColour( cr, COLOUR_GREY_4 );
      cr -> fill();
      
      // draw 8 rectangles for automation types
      cr->set_line_width(1.0);
      //cr->set_source_rgb (0.2,0.2,0.2);
      
      cr->rectangle(  7,  7, 50, 42 );
      cr->rectangle( 62,  7, 50, 42 );
      cr->rectangle(117,  7, 50, 42 );
      cr->rectangle(172,  7, 50, 42 );
      
      cr->rectangle(  7, 55, 50, 42 );
      cr->rectangle( 62, 55, 50, 42 );
      cr->rectangle(117, 55, 50, 42 );
      cr->rectangle(172, 55, 50, 42 );
      
      cr->rectangle(  7,103,215, 20 );
      setColour( cr, COLOUR_GREY_4 );
      cr->fill();
      
      // up
      cr->move_to ( 7,49 );
      cr->line_to (57, 7 );
      cr->line_to (57,49 );
      cr->close_path();
      
      // up down
      cr->move_to (62   ,49 );
      cr->line_to (62+25, 7 );
      cr->line_to (62+50,49 );
      cr->close_path();
      
      // down
      cr->move_to ( 7,55 );
      cr->line_to (57,92 );
      cr->line_to (57,55 );
      cr->close_path();
      
      // down up
      cr->move_to (62   ,55 );
      cr->line_to (62+25,92 );
      cr->line_to (62+50,55 );
      cr->close_path();
      
      setColour( cr, COLOUR_BLUE_1, 0.2 );
      cr->fill_preserve();
      
      cr->set_line_width(1.9);
      setColour( cr, COLOUR_BLUE_1 );
      cr->stroke();
      
      float progressPx = 215 * progress;
      
      // progress
      cr->rectangle( 7, 103, progressPx, 20 );
      setColour( cr, COLOUR_BLUE_1, 0.2 );
      cr->fill();
      cr->move_to( 7+progressPx, 103 );
      cr->line_to( 7+progressPx, 123 );
      setColour( cr, COLOUR_BLUE_1 );
      cr->set_line_width(3.5);
      cr->stroke();
      
      // outlines of squares
      cr->rectangle(  7,  7, 50, 42 );
      cr->rectangle( 62,  7, 50, 42 );
      cr->rectangle(117,  7, 50, 42 );
      cr->rectangle(172,  7, 50, 42 );
      
      cr->rectangle(  7, 55, 50, 42 );
      cr->rectangle( 62, 55, 50, 42 );
      cr->rectangle(117, 55, 50, 42 );
      cr->rectangle(172, 55, 50, 42 );
      
      cr->rectangle(  7,103,215, 20 );
      setColour( cr, COLOUR_GREY_2 );
      cr->set_line_width(2.2);
      cr->stroke();
      
      setColour( cr, COLOUR_GREEN_1, 0.3 );
      setColour(cr, COLOUR_GREY_1 );
      cr->stroke();
      
      /*
      // outline
      setColour(cr, COLOUR_GREY_3 );
      cr->rectangle( x, y , xSize, ySize );
      cr->set_line_width(3);
      cr->stroke();
      
      std::string name = "Start";
      std::string mid = "Middle";
      std::string nameStop = "Stop";
      
      drawMarker(cr, 0.f, COLOUR_GREEN_1, MARKER_START, name);
      
      //drawMarker(cr, 0.5f, COLOUR_PURPLE_2, MARKER_MIDDLE, mid);
      
      drawMarker(cr, 1.f, COLOUR_RECORD_RED, MARKER_END, nameStop );
      */
    }
  return true;
} // on_expose_event()

void GAutoMove::drawNoBuffer( Cairo::RefPtr<Cairo::Context> cr )
{
  int x = 10;
  int y = 22;
  
  int xSize = width - 2*x;
  int ySize = height - y;
  
  //draw X shape over full widget graph size:
  cr->move_to( x        , y         );
  cr->line_to( x + xSize, y + ySize );
  
  cr->move_to( x        , y + ySize );
  cr->line_to( x + xSize, y         );
  
  setColour(cr, COLOUR_GREY_2 );
  cr->stroke();
  
  // outline
  setColour(cr, COLOUR_GREY_3 );
  cr->rectangle( x, y , xSize, ySize );
  cr->set_line_width(3);
  cr->stroke();
}

/*
void GAutoMove::drawMarker(Cairo::RefPtr<Cairo::Context> cr, float position, Colour col, MarkerType type, std::string& name)
{
  int x = 10;
  int y = 22;
  
  // marks for start
  int markXsize = 40;
  int markX = x + position * ((width-markXsize) - x * 2);
  int markY = y / 2;
  
  int markYsize = y - 6;
  
  setColour(cr, col);
  if ( type == MARKER_START || type == MARKER_MIDDLE )
    cr->rectangle( markX, markY, 1, height);
  else
    cr->rectangle( markX + markXsize, markY, 1, height);
  cr->fill();
  
  setColour(cr, col);
  cr->rectangle( markX, markY, markXsize, markYsize );
  cr->fill();
  
  if ( type == MARKER_START || type == MARKER_MIDDLE )
  {
    cr->move_to( markX + markXsize, markY );
    cr->line_to( markX + markXsize + 9, markY + markYsize/2 );
    cr->line_to( markX + markXsize, markY + markYsize );
    cr->close_path();
    cr->fill();
  }
  else
  {
    cr->move_to( markX    , markY               );
    cr->line_to( markX - 9, markY + markYsize/2 );
    cr->line_to( markX    , markY + markYsize   );
    cr->close_path();
    cr->fill();
  }
  
  cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
  cr->set_font_size ( 13 );
  cr->move_to ( markX + 3, y + 1 );
  setColour(cr, COLOUR_GREY_4);
  cr->show_text ( name );
}
*/

void GAutoMove::redraw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

bool GAutoMove::on_button_press_event(GdkEventButton* event)
{
  if ( event->x > 7  && event->y > 7 &&
       event->x < 57 && event->y < 49 )
  {
    cout << "CLICK on UP!!" << endl;
    EngineEvent* x = new EngineEvent();
    x->setAutomoveType( -1, AUTOMOVE_TYPE_UP );
    top->toEngineQueue.push(x);
    
  }
  else
  {
    cout << "CLICK on OFFFFF!!" << endl;
    EngineEvent* x = new EngineEvent();
    x->setAutomoveType( -1, AUTOMOVE_TYPE_NONE );
    top->toEngineQueue.push(x);
  }
  
  /*
  if( event->button == 3 )
  {
    std::cout << "Event Type: " << event->type << ". Event Button: " << event->button << "." << std::endl;
    pMenu.popup(event->button, event->time);
  }
  */
  
  return true;
}
