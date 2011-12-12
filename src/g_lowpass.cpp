
#include "g_lowpass.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

int GLowPass::privateID = 0;

GLowPass::GLowPass(GuiStateStore* s)
{
  ID = privateID++;
  
  stateStore = s;
  
  cutoff = 0.40;
  q = 1.0;
  
  mouseDown = false;
  
  //std::cout << "Enterin GLowPass contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GLowPass::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GLowPass::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GLowPass::onMouseMove ) );
  
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
    
    bool active = true;
    
    int x = 10;
    int y = 22;
    xSize = 225;
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
    
    // draw "frequency guides"
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
    
    // move to bottom left, draw line to middle left
    cr->move_to( x , y + ySize );
    cr->line_to( x , y + (ySize/2));
    cr->line_to( xSize* (cutoff - 0.4) , y + (ySize/2) ); // start of curve
    
    cr->curve_to( xSize* (cutoff -0.1), y+(ySize*0.5),   // control point 1
                  xSize* (cutoff - 0.05), y+(ySize*0.1),   // control point 2
                  xSize* cutoff        , y+(ySize*0.1));  // end of curve
    
    cr->curve_to( xSize* (cutoff + 0.1 ), y+(ySize*0.15),  // control point 1
                  xSize* (cutoff + 0.15), y+(ySize*0.3), // control point 2
                  xSize* (cutoff + 0.15 ), y+(ySize)   ); // end of curve on floor
    
    setColour(cr, COLOUR_BLUE_1, 0.2 );
    cr->close_path();
    cr->fill_preserve();
    
    // stroke cutoff line
    cr->set_line_width(2.5);
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // click center
    setColour(cr, COLOUR_ORANGE_1, 0.9 );
    cr->arc( xSize*cutoff, ySize*q, 7, 0, 6.2830 );
    cr->stroke();
    
    // dials
    Dial(cr, active, 30, 170, 0.4 , DIAL_MODE_NORMAL);
    
    Dial(cr, active, 80, 170, 0.8 , DIAL_MODE_NORMAL);
    
    
    // outline
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(3);
    
    if ( active )
      setColour(cr, COLOUR_GREY_2 );
    else
      setColour(cr, COLOUR_GREY_3 );
    cr->stroke();
    
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

bool GLowPass::onMouseMove(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    cutoff = event->x / float(xSize);
    q = event->y / float(ySize);
    redraw();
    std::cout << "GLowPass: Cutoff = " << cutoff << std::endl;
  }
}

bool GLowPass::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    mouseDown = true;
    
    return true; //It's been handled.
  }
  else
    return false;
}

bool GLowPass::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GLowPass::~GLowPass()
{
}
