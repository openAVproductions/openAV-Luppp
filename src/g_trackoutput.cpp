
#include "g_trackoutput.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

int TrackOutput::privateID = 0;

TrackOutput::TrackOutput(GuiStateStore* s)
{
  ID = privateID++;
  
  stateStore = s;
  
  //std::cout << "Enterin TrackOutput contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &TrackOutput::on_button_press_event) );
  
  set_size_request(74,102);
}

bool TrackOutput::redraw()
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


bool TrackOutput::on_expose_event(GdkEventExpose* event)
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
    cr->set_source_rgb(0.1 , 0.1 , 0.1 );
    cr->fill();
    
    
    TrackOutputState* state = &stateStore->trackoutputState.at(ID);
    
    if ( state->selected )
      setColour(cr, COLOUR_GREY_3 );
    else
      setColour(cr, COLOUR_GREY_4 );
    
    cr->rectangle(0, 0, 74, 102);
    cr->fill();
    
    Dial(cr,true,7,4,state->pan,DIAL_MODE_PAN); // pan
    Mute(cr, 9  , 41 , state->ID, state->mute ); // mute button
    Solo(cr, 9  , 68 , state->ID, state->solo ); // solo button
    Rec (cr, 9  , 85 , state->ID, state->rec  ); // rec button
    Fader(cr,46 , 4  , state->volume, 0, 0 ); // fader
    
    if ( state->selected )
    {
      cr->rectangle(0, -10, 74, 200);
      setColour( cr, COLOUR_PURPLE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
    
  }
  return true;
}

void TrackOutput::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void TrackOutput::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool TrackOutput::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    
    return true; //It's been handled.
  }
  else
    return false;
}

TrackOutput::~TrackOutput()
{
}
