
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

#include "g_masteroutput.hpp"

#include "g_widgets.hpp"

#include <cmath>

using namespace std;
using namespace Luppp;

int GMasterOutput::privateID = 0;

GMasterOutput::GMasterOutput(Top* t, GuiStateStore* s)
{
  ID = privateID++;
  
  top = t;
  stateStore = s;
  
  mouseX = -1;
  mouseY = -1;
  
  try {
    headphoneImage = Gdk::Pixbuf::create_from_file("headphones.png");
  }
  catch (Glib::FileError e) {
    std::cout << "Gtk::FileError loading headphones file: " << e.what() << std::endl;
    exit(1);
  }
  
  //std::cout << "Enterin GMasterOutput contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GMasterOutput::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GMasterOutput::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GMasterOutput::onMouseMove ) );
  
  volume    = 0.000;
  pan       = 0.f;
  elevation = 0.5f;
  
  headphonePflSelect = false;
  headphonesVolume = 0.f;
  
  clickedWidget = CLICKED_WIDGET_NONE;
  
  set_size_request(74*3,130);
}

bool GMasterOutput::redraw()
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

void GMasterOutput::setVolume(float vol)
{
  cout << "GMasterOutput::setVolume() " << endl;
  volume = vol;
  redraw();
}


bool GMasterOutput::on_expose_event(GdkEventExpose* event)
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
        event->area.width, 130);
    cr->set_source_rgb(0.1 , 0.1 , 0.1 );
    setColour(cr, COLOUR_GREY_4 );
    cr->fill();
    
    
    TrackOutputState* state = new TrackOutputState(); // = &stateStore->trackoutputState.at(0);
    
    cr->rectangle   (155, 4, 12, 94);
    cr->rectangle   (170, 4, 12, 94);
    cr->rectangle   (185, 4, 12, 94);
    cr->rectangle   (200, 4, 12, 94);
    
    // setup & paint fader gradient
    cairo_pattern_t *pat;
    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0, 400.0);
    cairo_pattern_add_color_stop_rgb(pat, 0,   0 / 255.f, 153 / 255.f, 255 / 255.f );
    cairo_pattern_add_color_stop_rgb(pat, 1,  26 / 255.f,  26 / 255.f,  26 / 255.f );
    cairo_set_source(cr->cobj(), pat);
    cairo_fill      (cr->cobj());
    cairo_pattern_destroy (pat);    
    
    // draw fader  <|
    float playheadX = 195 + 12;
    float playheadY = 4 + (94 * ( 1.f - volume));
    
    setColour(cr, COLOUR_ORANGE_1 );
    cr->set_line_width(1.0);
    cr->move_to( playheadX, playheadY );
    cr->line_to( playheadX + 10, playheadY + 5.5 );
    cr->line_to( playheadX + 10, playheadY - 5.5 );
    cr->close_path();
    cr->fill_preserve();
    
    
    // draw fader  |>
    float playheadX2 = 160;
    setColour(cr, COLOUR_ORANGE_1 );
    cr->set_line_width(1.0);
    cr->move_to( playheadX2, playheadY );
    cr->line_to( playheadX2 - 10, playheadY + 5.5 );
    cr->line_to( playheadX2 - 10, playheadY - 5.5 );
    cr->close_path();
    cr->fill_preserve();
    setColour(cr, COLOUR_ORANGE_1 );
    cr->stroke();
    
    // line between faders
    cr->move_to( playheadX2, playheadY );
    cr->line_to( playheadX , playheadY );
    cr->set_line_width(2.7);
    cr->stroke();
    
    // draw rotation widget
    float rotation  = pan;
    
    // dashed cross lines
    std::valarray< double > dashes(2);
    dashes[0] = 3.0;
    dashes[1] = 3.0;
    cr->set_dash (dashes, 0.0);
    cr->set_line_width(2);
    setColour(cr, COLOUR_GREY_3);
    cr->move_to(7   ,7   );
    cr->line_to(7+90,7+90);
    cr->move_to(7   ,7+90);
    cr->line_to(7+90,7   );
    cr->stroke();
    cr->unset_dash();
    
    // elevation line
    cr->move_to( 7, 7 + 90 );
    cr->line_to( 7, 7 + 90 * (1-elevation) );
    cr->line_to( 95 + 7, 7 + 90 * (1-elevation));
    cr->line_to( 90 + 7, 7 + 90);
    cr->close_path();
    setColour(cr, COLOUR_BLUE_1, 0.2);
    cr->fill_preserve();
    setColour(cr, COLOUR_BLUE_1);
    cr->stroke();
    
    // blanking circle background colour
    cr->arc( 45 + 7, 45 + 7, 57, 0, 6.282);
    setColour(cr, COLOUR_GREY_4);
    cr->set_line_width(25);
    cr->stroke();
    
    // whole circle grey
    cr->arc( 45 + 7, 45 + 7, 45, 0, 6.282);
    setColour(cr, COLOUR_GREY_3);
    cr->set_line_width(3);
    cr->stroke();
    
    // rotation line
    cr->move_to(45 + 7, 45 + 7);
    float tmpRot = 6.282 * rotation - 3.1415/2.f;
    cr->arc( 45 + 7, 45 + 7, 45, tmpRot, tmpRot + 0.00001);
    setColour(cr, COLOUR_GREY_1);
    cr->set_line_width(2.8);
    cr->stroke();
    
    tmpRot = 6.282 * rotation;
    
    // rotation head
    float tmpX = sin ( tmpRot ) * 45;
    float tmpY = cos ( tmpRot ) * -45;
    
    //cout << "Rotation = " << rotation << "  Arc @ " << tmpX + 7 << ", " << tmpY + 7 << endl;
    
    cr->arc( 7 + 45 + tmpX, 7 + 45 + tmpY, 7, 0, 6.282);
    setColour(cr, COLOUR_BLUE_1);
    cr->fill_preserve();
    setColour(cr, COLOUR_GREY_3);
    cr->stroke();
    
    // inner circle grey
    cr->arc( 45 + 7, 45 + 7, 4, 0, 6.282);
    setColour(cr, COLOUR_GREY_4);
    cr->fill_preserve();
    cr->set_line_width(2);
    setColour(cr, COLOUR_GREY_3);
    cr->stroke();
    
    bool pfl = headphonePflSelect;
    
    // headphone monitoring system
    
    setColour(cr, COLOUR_BLUE_1, 0.2);
    cr->rectangle( 100, 75, 20, 20 );
    cr->fill_preserve();
    setColour(cr, COLOUR_BLUE_1);
    cr->stroke();
    
    cr->move_to( 105, 80 );
    cr->line_to( 115, 85 );
    cr->line_to( 105, 90 );
    cr->close_path();
    cr->fill();
    
    setColour(cr, COLOUR_RECORD_RED, 0.2);
    cr->rectangle( 125, 75, 20, 20 );
    cr->fill_preserve();
    setColour(cr, COLOUR_RECORD_RED);
    cr->stroke();
    
    //setColour(cr, COLOUR_BACKGROUND );
    cr->arc( 135, 85, 5, 0, 6.282);
    cr->fill();
    
    
    headphoneImage->render_to_drawable(	get_window() ,
                                        get_style()->get_black_gc(),
                                        0, 0,
                                        108, 7,
                                        -1, -1,
                                        Gdk::RGB_DITHER_NONE, 0, 0);
    
    // PFL custom output dial
    bool active = true;
    
    int xc = 108 + 18;
    int yc = 14 + 46;
    
    float radius = 14;
    
    cr->set_line_cap( Cairo::LINE_CAP_ROUND );
    cr->set_line_join( Cairo::LINE_JOIN_ROUND);
    cr->set_line_width(2.8);
    
    // Arc Angle Value
    cr->set_line_width(2.4);
    cr->move_to(xc,yc);
    cr->set_source_rgba( 0,0,0,0 );
    cr->stroke();
    
      setColour(cr, COLOUR_GREY_3 );
    cr->arc(xc,yc, radius, 2.46, 0.75 );
    cr->move_to(xc,yc);
    cr->stroke();
    
    cr->set_line_width(2.8);
    float angle;
    
    angle = 2.46 + (4.54 * headphonesVolume);
    if ( active )
      setColour(cr, COLOUR_ORANGE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->set_line_width(1.7);
    cr->arc(xc,yc, 13, 2.46, angle );
    cr->line_to(xc,yc);
    cr->stroke();
    cr->arc(xc,yc, 17, 2.46, angle );
    cr->line_to(xc,yc);
    cr->stroke();
    
    
    
  }
  return true;
}

void GMasterOutput::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GMasterOutput::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GMasterOutput::on_button_press_event(GdkEventButton* event)
{
  // reset click, will be set again if needed
  clickedWidget = CLICKED_WIDGET_NONE;
  
  if ( event->x > 152 ) // faders
  {
    clickedWidget = CLICKED_WIDGET_FADER;
    mouseX = event->x;
    mouseY = event->y;
    
    volume = 1 - ((event->y-4) / 94.f);
    cout << "Click volume : " << volume << endl;
    redraw();
  }
  else if ( event->x > 100 && event->x < 120 &&
            event->y >  75 && event->y <  95 )
  {
    cout << "SNAPSHOT RESTORE" << endl;
    EngineEvent* x = new EngineEvent();
    x->setTrackBacktrackRestore(0);
    top->toEngineQueue.push(x);
  }
  else if ( event->x > 125 && event->x < 145 &&
            event->y >  75 && event->y <  95 )
  {
    // Restore snapshot button clicked
    cout << "Restore Snapshot clicked" << endl;
    EngineEvent* x = new EngineEvent();
    x->setTrackBacktrackTake(0);
    top->toEngineQueue.push(x);
  }
  else if ( event->x >   7 && event->x <  97 &&
            event->y >   7 && event->y <  97 )
  {
    if( event->button == 1 ) {
      clickedWidget = CLICKED_WIDGET_ROTATE_PAN;
      pan = 1 - ((event->y-4) / 94.f);
      cout << "Clicked ROTATE_PAN " << pan << endl;
      redraw();
    }
    else if ( event->button == 3 ) {
      clickedWidget = CLICKED_WIDGET_ROTATE_ELEVATION;
      elevation = 1 - ((event->y-4) / 94.f);
      cout << "Clicked ROTATE_ELEVATION " << elevation << endl;
      redraw();
    }
  }
  else if ( event->x > 110 && event->x < 145 &&
            event->y > 40  && event->y < 75 )
  {
    cout << "DIAL CLICKED!!" << endl;
    clickedWidget = CLICKED_WIDGET_DIAL;
    
    headphonesDialClickOffset = event->y;
    
    headphonesDialClickVolume = headphonesVolume;
    
    cout << "headphonesDialClickOffset: " << headphonesDialClickOffset << ", headphonesVolume: "
         << headphonesVolume << endl;
          
    
    redraw();
  }
  else
  {
    clickedWidget = CLICKED_WIDGET_NONE;
  }
  
  /*
  // update Engine that we have a different track selected:
  EngineEvent* x = new EngineEvent();
  x->setTrackSelectDevice(ID, 0);
  top->toEngineQueue.push(x);
  */
  
  return true;
}
bool GMasterOutput::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE && event->button == 1 )
  {
    mouseX = -1;
    mouseY = -1;
  }
  
  clickedWidget = CLICKED_WIDGET_NONE;
  
  return true;
}

bool GMasterOutput::onMouseMove(GdkEventMotion* event)
{
  
  if ( clickedWidget == CLICKED_WIDGET_FADER )
  {
    volume = 1 - ((event->y-4) / 94.f);
    if ( volume > 1.f )
      volume = 1.f;
    else if ( volume < 0.f )
      volume = 0.f;
    
    redraw();
    
    EngineEvent* x = new EngineEvent();
    x->setMixerVolume( -1, volume);
    top->toEngineQueue.push(x);
  }
  else if ( clickedWidget == CLICKED_WIDGET_DIAL )
  {
    float currentDrag = (event->y - headphonesDialClickOffset) / 94.f;
    cout << "Offset: " << headphonesDialClickOffset << " current drag " << currentDrag
         << "  headphonesDialClickVolume " << headphonesDialClickVolume;
    
    if ( headphonesDialClickVolume > 1.f )
      headphonesDialClickVolume = 1.f;
    else if ( headphonesDialClickVolume < 0.f )
      headphonesDialClickVolume = 0.f;
    
    headphonesVolume = headphonesDialClickVolume - currentDrag;
    
    cout << "  final vol " << headphonesVolume << endl;
    
    if ( headphonesVolume > 1.f )
      headphonesVolume = 1.f;
    else if ( headphonesVolume < 0.f )
      headphonesVolume = 0.f;
    
    EngineEvent* x = new EngineEvent();
    x->setMixerVolume( -2, headphonesVolume);
    top->toEngineQueue.push(x);
    
    redraw();
  }
  else if ( clickedWidget == CLICKED_WIDGET_ROTATE_PAN )
  {
    pan = 1 - ((event->y-4) / 94.f);
    pan = pan - ((int)pan); // remove anything above 1
    cout << "move ROTATE_PAN " << pan << endl;
    redraw();
  }
  else if (clickedWidget == CLICKED_WIDGET_ROTATE_ELEVATION)
  {
    elevation = 1 - ((event->y-4) / 94.f);
    if ( elevation > 1.f )
      elevation = 1.f;
    else if ( elevation < 0.f )
      elevation = 0.f;
    cout << "move ROTATE_ELEVATION " << elevation << endl;
    redraw();
  }
  
  return true;
  
}

GMasterOutput::~GMasterOutput()
{
}
