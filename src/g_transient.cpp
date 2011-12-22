
#include "g_transient.hpp"

#include "top.hpp"
#include "g_statestore.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GTransient::GTransient(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  cutoff = 0.40;
  q = 1.0;
  
  mouseDown = false;
  
  //std::cout << "Enterin GTransient contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GTransient::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GTransient::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GTransient::onMouseMove ) );
  
  xSize = 225;
  
  set_size_request(130, 216);
}

bool GTransient::redraw()
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


bool GTransient::on_expose_event(GdkEventExpose* event)
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
    //std::cout << "Transient ID " << ID << " getting state now" << endl;
    float attack = stateStore->effectState.at(ID).values[0] * 2 - 1;
    float sustain = stateStore->effectState.at(ID).values[4] * 2 - 1;
    
    
    bool active = stateStore->effectState.at(ID).active;
    
    int x = 10;
    int y = 22;
    xSize = 110;
    ySize = 95;
    
    // background whole
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle(0, 0, 130, 216);
    cr->fill();
    
    // inner square
    setColour(cr, COLOUR_GREY_4 );
    cr->rectangle(x, y, xSize, ySize);
    cr->fill();
    
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
    
    Dial(cr, active, x + 30, y + 130, attack , DIAL_MODE_PAN);
    Dial(cr, active, x + 30, y + 170, sustain, DIAL_MODE_PAN);
    
    std::cout << "Attack: " << attack << " Sustain: " << sustain << std::endl;
    
    float pi = 3.1415 / 2.f;
    
    // range scale to 0 -> 1, then invert & * by pi
    float a = pi *   ( 1 - (attack + 1) / 2.f);
    float s = pi *   ( 1 - (sustain+ 1) / 2.f);
    
    std::cout << "A: " << a << " S: " << s << std::endl;
    
    float x1 = x;
    float y1 = y + ySize;
    
    float x2 = x + xSize;
    float y2 = y;
    
    // draw linear transient line
    setColour(cr, COLOUR_GREY_2, 0.2 );
    cr->move_to (x1,y1);
    cr->line_to (x2,y2);
    cr->stroke();
    
    float distX = sqrt( ((x2 - x1)*(x2 - x1)) ) / 3.f;
    float distY = sqrt( ((y2 - y1)*(y2 - y1)) ) / 3.f;
    
    std::cout << "a = " << a << "  dX = " << distX << " dY = " << distY << std::endl;
    
    cr->move_to( x1, y1 );
    
    float tx1 = x1 + distX * sin(a);
    float ty1 = y1 - distY * cos(a);
    //std::cout << "tX1 = " << tx1 - x << "  tY1 = " << ty1 - y << std::endl;
    
    float tx2 = x2 - (distX * cos(a));
    float ty2 = y2 + (distY * sin(a));
    std::cout << "tX2 = " << tx2 - x << "  tY2 = " << ty2 - y << std::endl;
    
    cr->curve_to( tx1, ty1,
                  tx2, ty2,
                  x2 , y2 );
    
    if ( active )
      setColour(cr, COLOUR_BLUE_1);
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    
    cr->set_line_width(2);
    cr->stroke();
    
    // CP circles
    cr->arc(tx1*0.9+3, ty1*0.9+3, 3, 0, 6.2830 );
    cr->arc(tx2*0.9+3, ty2*0.9+3, 3, 0, 6.2830 );
    
    if ( active )
      setColour(cr, COLOUR_ORANGE_1);
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    cr->fill();
    
    Dial(cr, active, x + xSize*0.5, 125, attack, DIAL_MODE_PAN);
    
    // outline
    setColour(cr, COLOUR_GREY_2 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(4);
    cr->stroke();
    
    TitleBar(cr, 0, 0, 130, 216, "Transient", active);
    
    /*
    if ( selected )
    {
      cr->rectangle(x, y, xSize, ySize);
      setColour( cr, COLOUR_ORANGE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
    */
  }
  return true;
}

void GTransient::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GTransient::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GTransient::onMouseMove(GdkEventMotion* event)
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
    std::cout << "GTransient: Cutoff = " << cutoff << "  Q: " << q << "  X, Y: " << event->x << ", " << event->y << std::endl;
  }
}

bool GTransient::on_button_press_event(GdkEventButton* event)
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
      
      q = evY / float(ySizeize);
      x = new EngineEvent();
      x->setPluginParameter(0,0,1, q );
      top->toEngineQueue.push(x);
      redraw();
      */
    }
    
    if ( event->y < 20 )
    {
      std::cout << "GTransient Enable / Disable click event!" << std::endl;
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(0,0,1 );
      top->toEngineQueue.push(x);
    }
    
    return true; //It's been handled.
  }
  else
    return false;
}

bool GTransient::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GTransient::~GTransient()
{
}
