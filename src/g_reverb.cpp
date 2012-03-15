
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

#include "g_reverb.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GReverb::GReverb(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  cutoff = 0.40;
  q = 0.0;
  
  mouseDown = false;
  
  //std::cout << "Enterin GReverb contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GReverb::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GReverb::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GReverb::onMouseMove ) );
  
  xSize = 110;
  
  set_size_request(75, 37);
}

bool GReverb::redraw()
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


bool GReverb::on_expose_event(GdkEventExpose* event)
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
    
    // update value from stateStore
    bool active     = stateStore->effectState.at(ID).active;
    
    float decayTime = stateStore->effectState.at(ID).values[0];
    float dryWet    = stateStore->effectState.at(ID).values[1];
    float highDamp  = stateStore->effectState.at(ID).values[2];
    float preDelay  = stateStore->effectState.at(ID).values[3];
    
    int x = 10;
    int y = 22;
    xSize = 110;
    ySize = 95;
    
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
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( x + ((xSize / 4.f)*i), y );
      cr->line_to( x + ((xSize / 4.f)*i), y + ySize );
    }
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( x       , y + ((ySize / 4.f)*i) );
      cr->line_to( x +xSize, y + ((ySize / 4.f)*i) );
    }
    cr->stroke();
    cr->unset_dash();
    
    // draw from bottom left up to dryWet point
    cr->move_to( x, y + ySize );
    cr->line_to( x, y + (ySize*0.75) );
    
    // dry wet line
    cr->line_to( x + (xSize*0.25), y + ySize*0.25 + (ySize*0.5)*(1-dryWet) );
    
    // decay line
    cr->line_to( x + (xSize*0.4) + (xSize*0.6)*decayTime, y + (ySize*0.75) );
    
    // bottom right
    cr->line_to( x + xSize, y + (ySize*0.75) );
    cr->line_to( x + xSize, y + ySize );
    
    cr->close_path();
    
    if ( active )
      setColour(cr, COLOUR_BLUE_1, 0.2 );
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    cr->set_line_width(2.5);
    cr->fill_preserve();
    
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
    
    // click center (range = 1/2 the range of the widget)
    if ( active )
      setColour(cr, COLOUR_ORANGE_1, 0.9 );
    else
      setColour(cr, COLOUR_GREY_1, 0.9 );
    
    float xArc = x + (xSize * 0.25) + (xSize*0.5) * decayTime;
    float yArc = y + (ySize * 0.75) - (ySize*0.5) * dryWet;
    
    //cout << " Arc x,y : " << xArc << ", " << yArc <<endl;
    cr->arc(xArc, yArc, 6 + 5 * preDelay , 0, 6.2830 );
    cr->stroke();
    
    
    // dials
    Dial(cr, active, 22, 125, decayTime         , DIAL_MODE_NORMAL);
    Dial(cr, active, 68, 125, dryWet            , DIAL_MODE_NORMAL);
    Dial(cr, active, 22, 165, preDelay          , DIAL_MODE_NORMAL);
    Dial(cr, active, 68, 165, highDamp          , DIAL_MODE_NORMAL);
    
    // outline
    setColour(cr, COLOUR_GREY_2 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(4);
    cr->stroke();
    
    TitleBar(cr, 0,0 , 250, 216, "Reverb", active);
    
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

void GReverb::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GReverb::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GReverb::onMouseMove(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    if ( (event->x > 50) && (event->x < 216) )
    {
      cutoff = event->x / float(xSize);
      
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(0,0,0, cutoff);
      top->toEngineQueue.push(x);
    }
    
    if ( (event->y > 35) && (event->y < 103) )
    {
      q = event->y / float(ySize);
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(0,0,1, q );
      top->toEngineQueue.push(x);
    }
    redraw();
    std::cout << "GReverb: Cutoff = " << cutoff << "  Q: " << q << "  X, Y: " << event->x << ", " << event->y << std::endl;
  }
}

bool GReverb::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    int x = 10;
    int y = 22;
    xSize = 225;
    ySize = 95;
    
    // graph area
    if ( (event->x > 10) && (event->x < 235) &&
         (event->y > 22) && (event->y < 117 ) )
    {
      /*
      std::cout << "graph area click!" << std::endl;
      mouseDown = true; // for pointer motion "drag" operations
      
      int evX = event->x;
      // inform engine of "click" and position co-efficents as such
      if ( evX < 50) evX = 50;
      if ( evX > 216)evX = 216;
      
      stateStore->cutoff = evX / float(xSize);
      cutoff = stateStore->cutoff;
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(0,0,0, cutoff );
      top->toEngineQueue.push(x);
      
      int evY = event->y;
      if (evY < 35 ) evY = 35;
      if (evY > 103) evY = 103;
      
      q = evY / float(ySize);
      x = new EngineEvent();
      x->setPluginParameter(0,0,1, q );
      top->toEngineQueue.push(x);
      redraw();
      */
    }
    
    if ( event->y < 20 )
    {
      std::cout << "GReverb Enable / Disable click event!" << std::flush;
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
      top->toEngineQueue.push(x);
      cout << " Done!" << endl;
    }
    
    return true; //It's been handled.
  }
  else
    return false;
}

bool GReverb::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GReverb::~GReverb()
{
}
