
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

#include "g_ampitchshift.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GAmPitchShift::GAmPitchShift(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  mouseDown = false;
  
  //std::cout << "Enterin GAmPitchShift contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GAmPitchShift::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GAmPitchShift::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GAmPitchShift::onMouseMove ) );
  
  set_size_request(75, 37);
}

bool GAmPitchShift::redraw()
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


bool GAmPitchShift::on_expose_event(GdkEventExpose* event)
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
      std::cout << "GAmPitchShift::expose() effectState.size() < ID!! cannot access values!" << std::endl;
      return true;
    }
    
    // update value from stateStore
    float shift = 1 - stateStore->effectState.at(ID).values[0];
    
    
    bool active = stateStore->effectState.at(ID).active;
    
    int x = 0;
    int y = 0;
    xSize = 75;
    ySize = 37;
    
    // works but a bit simple
    cr -> move_to( x        , y         );
    cr -> line_to( x + xSize, y         );
    cr -> line_to( x + xSize, y + ySize );
    cr -> line_to( x        , y + ySize );
    cr -> close_path();
    
    // Draw outline shape
    cr -> set_source_rgb (0.1,0.1,0.1);
    cr -> fill();
    
    // draw "guides"
    std::valarray< double > dashes(2);
    dashes[0] = 2.0;
    dashes[1] = 2.0;
    cr->set_dash (dashes, 0.0);
    cr->set_line_width(1.0);
    cr->set_source_rgb (0.4,0.4,0.4);
    for ( int i = 0; i < 3; i++ )
    {
      cr->move_to( x + ((xSize / 3.f)*i), y );
      cr->line_to( x + ((xSize / 3.f)*i), y + ySize );
    }
    for ( int i = 0; i < 3; i++ )
    {
      cr->move_to( x       , y + ((ySize / 3.f)*i) );
      cr->line_to( x +xSize, y + ((ySize / 3.f)*i) );
    }
    cr->stroke();
    cr->unset_dash();
    
    // vertical pitch change blocks
    cr->set_line_width(1.8);
    if ( active )
      setColour(cr, COLOUR_BLUE_1, 0.2 );
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    
    // left block
    cr->move_to(           0, ySize / 2.f );
    cr->line_to( xSize / 2.f, ySize / 2.f );
    cr->line_to( xSize / 2.f, ySize / 2.f + ((0.5-shift)*ySize) );
    cr->line_to(           0, ySize / 2.f + ((0.5-shift)*ySize) );
    cr->close_path();
    
    // right block
    cr->move_to( xSize / 2.f, ySize / 2.f );
    cr->line_to( xSize      , ySize / 2.f );
    cr->line_to( xSize      , ySize / 2.f - ((0.5-shift)*ySize) );
    cr->line_to( xSize / 2.f, ySize / 2.f - ((0.5-shift)*ySize) );
    cr->close_path();
    
    //cr->rectangle( xSize/4.f, ySize/2.f, xSize / 2.f, shift*ySize/2.f);
    cr->fill_preserve();
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // click center
    if ( active )
      setColour(cr, COLOUR_ORANGE_1, 0.9 );
    else
      setColour(cr, COLOUR_GREY_1, 0.9 );
    
    cr->set_line_width(2.7);
    cr->arc( (xSize/2.f) + ((0.5-shift) * xSize)  , ySize / 2.f, 7, 0, 6.2830 );
    cr->stroke();
    
    
    // dials
    //Dial(cr, active, 48, 125, delayTime, DIAL_MODE_NORMAL);
    //Dial(cr, active, 48, 165, thresh       , DIAL_MODE_NORMAL);
    
    // outline
    /*
    setColour(cr, COLOUR_GREY_2 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(4);
    cr->stroke();
    */
    
    //TitleBar(cr, 0,0 , 130, 216, "Pitch Shifter", active);
    
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

void GAmPitchShift::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GAmPitchShift::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GAmPitchShift::onMouseMove(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    float value = event->x / xSize;
    cout << "AmPitchShift Param 0 : " << value << endl;
    
    EngineEvent* x = new EngineEvent();
    x->setPluginParameter(ID, 0, 0, value);
    top->toEngineQueue.push(x);
  }
}

bool GAmPitchShift::on_button_press_event(GdkEventButton* event)
{
  if( event->button == 1 )
  {
      mouseDown = true; // for pointer motion "drag" operations
      
      float value = event->x / xSize;
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(ID, 0, 0, value);
      top->toEngineQueue.push(x);
  }
  else if ( event->button == 3 )
  {
    //std::cout << "GAmPitchShift Enable / Disable click event!" << std::endl;
    EngineEvent* x = new EngineEvent();
    x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
    top->toEngineQueue.push(x);
  }
  
  return true; //It's been handled.
}

bool GAmPitchShift::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GAmPitchShift::~GAmPitchShift()
{
}
