
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

#include "g_beatsmash.hpp"

#include "g_widgets.hpp"

#include <sstream>

using namespace std;
using namespace Luppp;

GBeatSmash::GBeatSmash(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  mouseDown = false;
  
  //std::cout << "Enterin GBeatSmash contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GBeatSmash::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GBeatSmash::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GBeatSmash::onMouseMove ) );
  
  set_size_request(74, 37);
  xSize = 74;
}

bool GBeatSmash::redraw()
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


bool GBeatSmash::on_expose_event(GdkEventExpose* event)
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
    
    if ( ID >= stateStore->effectState.size() )
    {
      std::cout << "GBeatSmash::expose() effectState.size() < ID!! cannot access values!" << std::endl;
      return true;
    }
    
    // update value from stateStore
    float delayTime = stateStore->effectState.at(ID).values[0];
    bool active = stateStore->effectState.at(ID).active;
    bool globalUnit = stateStore->effectState.at(ID).globalUnit;
    
    int x = 0;
    int y = 0;
    xSize = 75;
    ySize = 35;
    
    // fill bg
    cr -> move_to( x        , y         );
    cr -> line_to( x + xSize, y         );
    cr -> line_to( x + xSize, y + ySize );
    cr -> line_to( x        , y + ySize );
    cr -> close_path();
    cr -> set_source_rgb (0.1,0.1,0.1);
    cr -> fill();
    
    // draw bar, depending on delayTime
    if ( active )
      setColour(cr, COLOUR_BLUE_1, 0.2 );
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    cr->rectangle( 0, 8, delayTime * xSize, ySize - 16 );
    cr->fill_preserve();
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // click center
    if ( globalUnit )
    {
      if ( active )
        setColour(cr, COLOUR_GREEN_1, 0.9 );
      else
        setColour(cr, COLOUR_GREY_1,0.9 );
      cr->move_to( xSize * delayTime - 5, ySize*0.5 - 5 );
      cr->line_to( xSize * delayTime + 5, ySize*0.5 + 5 );
      cr->move_to( xSize * delayTime - 5, ySize*0.5 + 5 );
      cr->line_to( xSize * delayTime + 5, ySize*0.5 - 5 );
      cr->stroke();
    }
    else
    {
      if ( active )
        setColour(cr, COLOUR_ORANGE_1, 0.9 );
      else
        setColour(cr, COLOUR_GREY_1, 0.9 );
      cr->arc( xSize*delayTime, ySize*0.5, 7, 0, 6.2830 );
      cr->stroke();
    }
    
    // draw "guides"
    std::valarray< double > dashes(2);
    dashes[0] = 2.0;
    dashes[1] = 2.0;
    cr->set_line_width(2.0);
    setColour(cr, COLOUR_GREY_4 ); // first line bg colour
    
    for ( int i = 0; i < 4; i++ )
    {
      setColour(cr, COLOUR_GREY_3 );
      cr->move_to( x + ((xSize / 4.f)*i), y );
      cr->line_to( x + ((xSize / 4.f)*i), y + ySize );
      cr->set_dash (dashes, 0.0);
      cr->stroke();
      cr->unset_dash();
      
      // show number of delay
      cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
      cr->set_font_size ( 12 );
      cr->move_to ( x + 5 + ((xSize / 4.f)*i) , 4 + ySize / 2. );
      setColour(cr, COLOUR_GREY_3 );
      if ( active )
        setColour(cr, COLOUR_ORANGE_1 );
      stringstream s;
      s << i + 1;
      cr->show_text ( s.str() );
      cr->stroke();
      
      setColour(cr, COLOUR_GREY_3 );
    }
    cr->unset_dash();
  }
  return true;
}

void GBeatSmash::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GBeatSmash::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GBeatSmash::onMouseMove(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    EngineEvent* ee = new EngineEvent();
    ee->setPluginParameter(ID,0,0, event->x / xSize );
    top->toEngineQueue.push(ee);
    std::cout << "GBeatSmahs: New delayTime = " << event->x / xSize << std::endl;
  }
}

bool GBeatSmash::on_button_press_event(GdkEventButton* event)
{
  if( event->button == 1 )
  {
      float gain = event->x / xSize;
      cout << "GBeatsmash Clicked level: " << gain << endl;
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(ID, 0, 0, gain);
      top->toEngineQueue.push(x);
      mouseDown = true;
  }
  else if ( event->button == 3 ) // right click to enable disable
  {
    std::cout << "GBeatsmash Enable / Disable click event!" << std::endl;
    EngineEvent* x = new EngineEvent();
    x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
    top->toEngineQueue.push(x);
  }
  
  if ( event->type == GDK_2BUTTON_PRESS && event->button == 1 ) // double left click
  {
    EngineEvent* x = new EngineEvent();
    x->setPluginGlobalUnit( ID, !stateStore->effectState.at(ID).globalUnit );
    top->toEngineQueue.push(x);
  }
  
  return false;
}

bool GBeatSmash::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GBeatSmash::~GBeatSmash()
{
}
