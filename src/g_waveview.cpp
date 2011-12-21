#include "g_waveview.hpp"
#include <iostream>
#include <cairomm/context.h>
#include <sstream>

GWaveView::GWaveView()
{
  
  // Gives Exposure & Button presses to the widget.
  add_events(Gdk::EXPOSURE_MASK);
  add_events(Gdk::BUTTON_PRESS_MASK );
  
  // set default widget size
  set_size_request(400,100);
}

void GWaveView::setSample(const std::vector<float>& inSample)
{
  std::cout << "void setSample() inSamp.size() = " << inSample.size() << std::endl;
  
  for ( int i = 0; i < inSample.size(); i++ )
  {
    sample.push_back(inSample.at(i));
  }
  
  while ( sample.size() > 15000 )
    sample.erase( sample.begin(),sample.begin()+ 128);
}

GWaveView::~GWaveView()
{
}

bool GWaveView::on_expose_event(GdkEventExpose* event)
{
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    
    if(window)    // Only run if Window does exist
    {
      Gtk::Allocation allocation = get_allocation();
      const int width = allocation.get_width();
      const int height = allocation.get_height();
      
      // coordinates for the center of the window
      int xc, yc;
      xc = width / 2;
      yc = height / 2;
      
      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,event->area.width, event->area.height);
      cr->clip();
      
      //cr->move_to( width*(float(index)/sample.size())  , height/2);
      
      // loop for drawing each Point on the widget.
      for (long index=0; index <(long)sample.size(); index += 50) 
      {
        cr->line_to( width*(float(index)/sample.size())  ,(height/2)+ (float(sample.at(index)) * height)  );
      }
      cr->set_source_rgb(1.0,0.4,0.0 );
      cr->stroke();
    }
  return true;
} // on_expose_event()

void GWaveView::redraw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

bool GWaveView::on_button_press_event(GdkEventButton* event)
{
  sample.clear();
  
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    sample.push_back(0);
    return true; //It's been handled.
  }
  else
  {
    sample.push_back(1);
  }
  
  redraw();
  
  return true;
}
