

#include "bindings.h"

namespace Avtk
{

Bindings::Bindings( int _x, int _y, int _w, int _h, const char *_label ) :
        Fl_Button(_x, _y, _w, _h, _label)
{
  x = _x;
  y = _y;
  w = _w;
  h = _h;
  
  label = _label;
  
  highlight = false;
  mouseOver = false;
}


void Bindings::draw()
{
  if (damage() & FL_DAMAGE_ALL)
  {
    cairo_t *cr = Fl::cairo_cc();
    cairo_save( cr );
    
    int drawY = 0;
    for( int i = 0; i < 10; i++) // draw each binding
    {
      cairo_rectangle( cr, x+2, y + drawY, 100, 25 );
      drawY += 28;
    }
    cairo_set_source_rgba(cr, 0 / 255.f,  0 / 255.f ,  0 / 255.f, 0.4);
    cairo_fill(cr);
    
    cairo_restore( cr );
  }
}



void Bindings::resize(int X, int Y, int W, int H)
{
  Fl_Widget::resize(X,Y,W,H);
  x = X;
  y = Y;
  w = W;
  h = H;
  redraw();
}

/*
int Bindings::handle(int event)
{
  switch(event)
  {
    case FL_PUSH:
      highlight = 0;
      redraw();
      return 1;
    case FL_DRAG:
      {
        if ( Fl::event_state(FL_BUTTON1) )
        {
          if ( mouseClicked == false ) // catch the "click" event
          {
            mouseClickedX = Fl::event_x();
            mouseClickedY = Fl::event_y();
            mouseClicked = true;
          }
          
          float delta = (mouseClickedY - Fl::event_y() ) / float(h);
          // handle the x / y swap, and the inverting of direction (mouseX / Y relative)
          if ( orientationHorizontal )
            delta = ( Fl::event_x() - mouseClickedX ) / float(w);
          
          float valY = value();
          valY += delta;
          
          if ( valY > 1.0 ) valY = 1.0;
          if ( valY < 0.0 ) valY = 0.0;
          
          set_value( valY );
          
          mouseClickedX = Fl::event_x();
          mouseClickedY = Fl::event_y();
          redraw();
          do_callback();
        }
      }
      return 1;
    case FL_ENTER:
      return 1;
    case FL_RELEASE:
      if (highlight) {
        highlight = 0;
        redraw();
        do_callback();
      }
      mouseClicked = false;
      return 1;
    case FL_SHORTCUT:
      if ( test_shortcut() )
      {
        do_callback();
        return 1;
      }
      return 0;
    default:
      return Fl_Widget::handle(event);
  }
}
*/

} // namespace Avtk
