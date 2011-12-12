
#include "g_lowpass.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

int GLowPass::privateID = 0;

GLowPass::GLowPass(GuiStateStore* s)
{
  ID = privateID++;
  
  stateStore = s;
  
  //std::cout << "Enterin GLowPass contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GLowPass::on_button_press_event) );
  
  set_size_request(250, 216);
}

bool GLowPass::redraw()
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


bool GLowPass::on_expose_event(GdkEventExpose* event)
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
    
    std::cout << "StateStore = " << &stateStore << std::endl;
    
    /*
    GLowPassState* freqState = &stateStore->eqState;
    
    int dialSize = 30;
    
    bool active = true;
    
    
    // setup array of dials
    for ( int i = 0; i < 4; i++ )
    {
      for( int j = 0; j < 2; j++ )
      {
        float value = 0.f;
        if ( j % 2 == 0 )
          value = (freqState->gain[i] * 2) - 1;
        else
        {
          value = (freqState->cutoffFreq[i] - 0.5)*2;
        }
        Dial(cr, active, 30 + ( 50 * i)  , 130 + 40 * j , value , DIAL_MODE_PAN);
      }
    }
    
    FrequencyGraph(cr, active, 10, 22, 225, 95, *freqState );
    */
    
    TitleBar(cr, 0,0 , 250, 216, "Lowpass", false);
    
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

void GLowPass::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GLowPass::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GLowPass::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    
    return true; //It's been handled.
  }
  else
    return false;
}

GLowPass::~GLowPass()
{
}
