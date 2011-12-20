
#include "g_highpass.hpp"

#include "g_widgets.hpp"

using namespace std;
using namespace Luppp;

GHighPass::GHighPass(Top* t, GuiStateStore* s)
{
  ID = WidgetBase::getID();
  
  top = t;
  stateStore = s;
  
  cutoff = 0.0;
  q = 1.0;
  
  mouseDown = false;
  
  //std::cout << "Enterin GHighPass contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GHighPass::on_button_press_event) );
  signal_button_release_event().connect(sigc::mem_fun(*this, &GHighPass::on_button_release_event) );
  signal_motion_notify_event().connect( sigc::mem_fun( *this, &GHighPass::onMouseMove ) );
  
  set_size_request(250, 216);
  xSize = 225;
  
  redraw();
}

bool GHighPass::redraw()
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


bool GHighPass::on_expose_event(GdkEventExpose* event)
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
    
    //cout << "HighPass getting state ID " << ID << endl; 
    float cutoffRangeZeroOne = stateStore->effectState.at(ID).values[0];
    
    cutoff = (48.f / xSize) + (cutoffRangeZeroOne * 0.7541 );
    
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
    cr->move_to( x + xSize , y + ySize );
    cr->line_to( x + xSize , y + (ySize/2));
    
    
    int startHorizontalLine = xSize* (cutoff + 0.4);
    if ( startHorizontalLine > 235 )
      startHorizontalLine = 235;
      
    cr->line_to( startHorizontalLine, y + (ySize/2) ); // horizontal line to start of curve
    
    int xSize1CP1 = xSize* (cutoff +0.1);
    int xSize1CP2 = xSize* (cutoff +0.08);
    int xSize1End = xSize* cutoff;
    
    if ( xSize1CP1 > 235 )
      xSize1CP1 = 235;
    if ( xSize1CP2 > 235 )
      xSize1CP2 = 235;
    if ( xSize1End > 235 )
      xSize1End = 235;
    
    cr->curve_to( xSize1CP1, y+(ySize*0.5),   // control point 1
                  xSize1CP2, y+(ySize*0.3),   // control point 2
                  xSize1End, y+(ySize*0.3));  // end of curve 1, start curve 2
    
    int xSize2CP1 = xSize* (cutoff - 0.03);
    int xSize2CP2 = xSize* (cutoff - 0.08);
    int xSize2End = xSize* (cutoff - 0.15);
    
    if ( xSize2CP1 > 234 )
      xSize2CP1 = 234;
    if ( xSize2CP2 > 234 )
      xSize2CP2 = 234;
    if ( xSize2End > 234 )
      xSize2End = 234;
    
    cr->curve_to( xSize2CP1, y+(ySize*0.3),  // control point 1
                  xSize2CP2, y+(ySize*0.3), // control point 2
                  xSize2End, y+(ySize)   ); // end of curve on floor
    
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
    Dial(cr, active, 70, 140, cutoffRangeZeroOne, DIAL_MODE_NORMAL);
    Dial(cr, active, 150,140, q                 , DIAL_MODE_NORMAL);
    
    // outline
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(3);
    
    if ( active )
      setColour(cr, COLOUR_GREY_2 );
    else
      setColour(cr, COLOUR_GREY_3 );
    cr->stroke();
    
    TitleBar(cr, 0,0 , 250, 216, "Highpass", true);
    
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

void GHighPass::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void GHighPass::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool GHighPass::onMouseMove(GdkEventMotion* event)
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
    std::cout << "GHighPass: Cutoff = " << cutoff << "  Q: " << q << "  X, Y: " << event->x << ", " << event->y << std::endl;
  }
}

bool GHighPass::on_button_press_event(GdkEventButton* event)
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
    }
    
    if ( event->y < 20 )
    {
      std::cout << "GHighPass Enable / Disable click event!" << std::endl;
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(0,0,1 );
      top->toEngineQueue.push(x);
    }
    
    return true; //It's been handled.
  }
  else
    return false;
}

bool GHighPass::on_button_release_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_RELEASE  ) // && event->button == 3
  {
    mouseDown = false;
    
    return true; //It's been handled.
  }
  else
    return false;
}

GHighPass::~GHighPass()
{
}
