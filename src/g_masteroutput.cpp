
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
  
  //std::cout << "Enterin GMasterOutput contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GMasterOutput::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GMasterOutput::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GMasterOutput::onMouseMove ) );
  
  set_size_request(74*3,104);
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
        event->area.width, event->area.height);
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
    
    
    float value = 0.7f;
    
    // draw fader  <|
    float playheadX = 195 + 12;
    float playheadY = 4 + (92 * ( 1.f - value));
    
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
    float elevation = 0.5f;
    float rotation  = 0.f;
    
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
    
    // rotation head
    float tmpX = sin ( tmpRot ) * 90;
    float tmpY = cos ( tmpRot ) * 90;
    
    cr->rectangle ( 7 + tmpX, 7 + tmpY , 4, 4 );
    setColour(cr, COLOUR_BLUE_1);
    cr->stroke();
    
    // inner circle grey
    cr->arc( 45 + 7, 45 + 7, 4, 0, 6.282);
    setColour(cr, COLOUR_GREY_4);
    cr->fill_preserve();
    cr->set_line_width(2);
    setColour(cr, COLOUR_GREY_3);
    cr->stroke();
    
    bool pfl = true;
    
    // headphone monitoring system
    Solo(cr, 114, 60, 0,  pfl ); // solo button
    Solo(cr, 114, 82, 0, !pfl ); // solo button
    
    // PFL custom output dial
    bool active = true;
    
    int xc = 110 + 16;
    int yc = 14 + 22;
    
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
    if ( value < 0 )
      value = 0.f;
    //std::cout << "Dial NORMAL: value = " << value << std::endl;
    angle = 2.46 + (4.54 * value);
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
  
  /*
  if( event->type == GDK_BUTTON_PRESS && event->button == 1 )
  {
    // reset click, will be set again if needed
    clickedWidget = CLICKED_WIDGET_NONE;
    
    if ( event->x > 38 ) // fader
    {
      clickedWidget = CLICKED_WIDGET_FADER;
      mouseX = event->x;
      mouseY = event->y;
    }
    else if ( event->x > 5 && event->y > 39 && event->y < 39 + 23 ) // mute
    {
      EngineEvent* x = new EngineEvent();
      TrackOutputState* state = new TrackOutputState(); //  = &stateStore->trackoutputState.at(0);
      x->setTrackMute(ID, !state->mute);
      top->toEngineQueue.push(x);
    }
    else if ( event->x > 5 && event->y > 67 && event->y < 67 + 13 ) // solo
    {
      std::cout << "SOLO" << std::endl;
      EngineEvent* x = new EngineEvent();
      TrackOutputState* state = new TrackOutputState(); //  = &stateStore->trackoutputState.at(0);
      x->setTrackSolo(ID, !state->solo);
      top->toEngineQueue.push(x);
    }
    else if ( event->x > 5 && event->y > 85 && event->y < 85 + 13 ) // rec
    {
      std::cout << "REC" << std::endl;
      EngineEvent* x = new EngineEvent();
      TrackOutputState* state = new TrackOutputState(); //  = &stateStore->trackoutputState.at(0);
      x->setTrackRec(ID, !state->recEnable);
      top->toEngineQueue.push(x);
    }
    else if ( event->x > 3 && event->y > 7 && event->y < 37 )
    {
      std::cout << "DIAL" << std::endl;
      clickedWidget = CLICKED_WIDGET_DIAL;
    }
    
  }
  
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
  /*
  if ( clickedWidget == CLICKED_WIDGET_FADER )
  {
    TrackOutputState* state = new TrackOutputState(); //  = &stateStore->trackoutputState.at(0);
    
    float mouseYdelta = (mouseY - event->y) / 25.f;
    //std::cout << "MouseYdelta: " << mouseYdelta << std::endl;
    
    EngineEvent* x = new EngineEvent();
    
    // move volume relative to current value
    x->setMixerVolume(ID, mouseYdelta + state->volume);
    top->toEngineQueue.push(x);
    
    // reset mouseY
    mouseY = event->y;
  }
  else if ( clickedWidget == CLICKED_WIDGET_DIAL )
  {
    TrackOutputState* state = new TrackOutputState(); //  = &stateStore->trackoutputState.at(0);
    
    float mouseYdelta = (mouseY - event->y) / 225.f;
    std::cout << "MouseYdelta: " << mouseYdelta << std::endl;
    
    EngineEvent* x = new EngineEvent();
    
    // move volume relative to current value
    x->setTrackPan(ID, mouseYdelta + state->pan);
    top->toEngineQueue.push(x);
    
    // reset mouseY
    mouseY = event->y;
  }
  */
  
  return true;
  
}

GMasterOutput::~GMasterOutput()
{
}
