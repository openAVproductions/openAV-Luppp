
#include "g_limiter.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GLimiter::GLimiter(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  mouseDown = false;
  
  //std::cout << "Enterin GLimiter contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GLimiter::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GLimiter::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GLimiter::onMouseMove ) );
  
  set_size_request(130, 216);
}

bool GLimiter::redraw()
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


bool GLimiter::on_expose_event(GdkEventExpose* event)
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
      std::cout << "GLimiter::expose() effectState.size() < ID!! cannot access values!" << std::endl;
      return true;
    }
    
    // update value from stateStore
    float makeupZeroOne = 1 - stateStore->effectState.at(ID).values[0];
    float thresh = stateStore->effectState.at(ID).values[1];
    
    cout << "Limiter effectState[0] = " << stateStore->effectState.at(ID).values[0] << endl;
    
    float ratio = 1.f; // hack on compressor code, will be "inf" compression
    float makeup = (makeupZeroOne) * (ySize * 0.5);
    
    bool active = stateStore->effectState.at(ID).active;
    
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
    
    // draw "normal" line
    setColour(cr, COLOUR_GREY_3 );
    cr->set_line_width(2.5);
    cr->move_to( x , y + ySize );
    cr->line_to( x + xSize, y );
    cr->stroke();
    
    float xDist = 0.1 * xSize;
    float yDist = 0.1 * ySize;
    
    float xThresh = x + (xSize * 0.25) + (xSize*0.5) * thresh;
    float yThresh = y + (ySize * 0.25) + (ySize*0.5)*(1-thresh);
    
    float startx = xThresh - xDist;
    float starty = yThresh + yDist;
    
    float cp1x = xThresh;
    float cp1y = yThresh - makeup;
    
    float cp2x = xThresh;
    float cp2y = yThresh - makeup;
    
    float endx = xThresh + (xDist*1.2);
    float endy = yThresh - (yDist*1.2)*(1-ratio) - makeup;
    
    // move to bottom left, draw line to middle left
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->move_to( x , y + ySize - makeup );
    cr->line_to( startx, starty - makeup );
    cr->line_to(x + xSize, y - makeup );
    cr->line_to(x + xSize, y + ySize );
    cr->line_to(x , y + ySize );
    cr->close_path();
    
    if ( active )
      setColour(cr, COLOUR_BLUE_1, 0.2 );
    else
      setColour(cr, COLOUR_GREY_1, 0.2 );
    cr->close_path();
    cr->fill_preserve();
    
    // stroke gain line
    cr->set_line_width(2.5);
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // draw threshold as dotted orange line across widget
    cr->move_to(x, y+ ySize*0.25 + ySize*0.5*(1-thresh));
    cr->line_to(x + xSize, y+ ySize*0.25 + ySize*0.5 * (1-thresh) );
    
    if ( active )
      setColour(cr, COLOUR_RECORD_RED );
    else
      setColour(cr, COLOUR_GREY_1 );
    
    dashes[0] = 10;
    dashes[1] = 4;
    cr->set_dash (dashes, 0.0);
    cr->set_line_width(1.7);
    cr->stroke();
    cr->unset_dash();
    
    // gain line can go beyond the "graph area", and it would be hard to
    // clip values the right amout with the curve. Instead just cover it
    // with a "background coloured" rectangle
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle(0,0,130,y); // 130 from widget size
    cr->fill();
    
    
    // click center ( range = 1/2 the range of the widget
    if ( active )
      setColour(cr, COLOUR_ORANGE_1, 0.9 );
    else
      setColour(cr, COLOUR_GREY_1, 0.9 );
    float xArc = x + (xSize * 0.25) + (xSize*0.5) * thresh;
    float yArc = y + (ySize * 0.75)  - ySize*0.5* makeupZeroOne;
    
    //cout << " Arc x,y : " << xArc << ", " << yArc <<endl;
    cr->set_line_width(2.7);
    cr->arc(xArc, yArc, 7, 0, 6.2830 );
    cr->stroke();
    
    // dials
    Dial(cr, active, 48, 125, makeupZeroOne, DIAL_MODE_NORMAL);
    Dial(cr, active, 48, 165, thresh       , DIAL_MODE_NORMAL);
    
    // outline
    setColour(cr, COLOUR_GREY_2 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(4);
    cr->stroke();
    
    TitleBar(cr, 0,0 , 130, 216, "Limiter", active);
    
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

void GLimiter::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GLimiter::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GLimiter::onMouseMove(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    int x = 10;
    int y = 22;
    if ( (event->x > x + xSize*0.25) && (event->x < x + xSize*0.75) )
    {
      
      float gain = (event->x - (x + xSize*0.25)) / float(xSize*0.5);
      
      cout << "Limiter Gain : " << gain << endl;
      
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(ID, 1, 1, gain);
      top->toEngineQueue.push(x);
    }
    
    if ( (event->y > y + ySize*0.25) && (event->y < y+ySize*0.75) )
    {
      float limit = (event->y - (y + ySize*0.25)) / float(ySize*0.5);
      
      cout << "Limiter limit : " << limit << endl;
      
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(ID, 0, 0, limit);
      top->toEngineQueue.push(x);
    }
    //std::cout << "GLimiter: Cutoff = " << cutoff << "  Q: " << q << "  X, Y: " << event->x << ", " << event->y << std::endl;
  }
}

bool GLimiter::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    int x = 10;
    int y = 22;
    
    // graph area
    if ( (event->x > x) && (event->x < x + xSize) &&
         (event->y > y) && (event->y < y + ySize ) )
    {
      
      std::cout << "graph area click!" << std::flush;
      mouseDown = true; // for pointer motion "drag" operations
      
      cout << " x before = " << event->x;
      
      // clamp click coords to acceptable grid, then send EE
      if ( event->x < x + (xSize*0.25)){
        cout << "  X smaller than! Clipping  ";
        event->x = x + xSize*0.25;
      }
      else if (event->x > x + xSize*0.75) {
        event->x = x + xSize*0.75;
      }
      
      cout << "  x afterClip = " << event->x;
      
      float gain = (event->x - (x + xSize*0.25)) / float(xSize*0.5);
      cout << "  final Limiter Clicked Gain : " << gain << endl;
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(ID, 1, 1, gain);
      top->toEngineQueue.push(x);
      
      
      if (event->y < y + ySize*0.25) {
        event->y = y + ySize*0.25;
      }
      else if (event->y > y+ySize*0.75 ) {
        event->y = y + ySize*0.75;
      }
    
      float limit = (event->y - (y + ySize*0.25)) / float(ySize*0.5);
      cout << "Limiter Clicked limit : " << limit << endl;
      x = new EngineEvent();
      x->setPluginParameter(ID, 0, 0, limit);
      top->toEngineQueue.push(x);
      
    }
    
    if ( event->y < 20 )
    {
      std::cout << "GLimiter Enable / Disable click event!" << std::endl;
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
      top->toEngineQueue.push(x);
    }
    
    return true; //It's been handled.
  }
  else
    return false;
}

bool GLimiter::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GLimiter::~GLimiter()
{
}
