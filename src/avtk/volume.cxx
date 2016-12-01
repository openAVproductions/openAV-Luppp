/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "volume.hxx"

using namespace Avtk;

Volume::Volume(int _x, int _y, int _w, int _h, const char *_label ):
  Fl_Slider(_x, _y, _w, _h, _label)
{
  x = _x;
  y = _y;
  w = _w;
  h = _h;
  
  label = _label;
  
  mouseClickedX = 0;
  mouseClickedY = 0;
  mouseClicked = false;
  
  active = true;
  highlight = false;
  
  ampL = 0;
  ampR = 0;
  compress = 0;
  value( 0.78f );

  orientationHorizontal = false;
}


void Volume::fader( float f )
{
  // redraw on larger value change
  //if ( fabsf( value() - f ) > 0.025 )
  value( f );
}


void Volume::amplitude  (float aL, float aR)
{
  if ( aL < 0.1 ) aL = 0.f;
  if ( aR < 0.1 ) aR = 0.f;
  
  // only redraw if changed more than X amount
  if ( fabsf(ampL - aL) > 0.025 ||
       fabsf(ampR - aR) > 0.025 )
  {
    ampL = aL;
    ampR = aR;
    redraw();
  }
}


void Volume::compression(float c)
{
  compress = c;
  redraw();
}


void Volume::setOrientationHorizontal()
{
  orientationHorizontal = true;
  redraw();
}

void Volume::set_active(bool a)
{
  active = a;
  redraw();
}

void Volume::draw()
{
  if (damage() & FL_DAMAGE_ALL)
  {
    cairo_t *cr = Fl::cairo_cc();
    
    cairo_save( cr );
    
    cairo_set_line_width(cr, 1.5);
    
    // fill background
    cairo_rectangle( cr, x, y, w, h);
    cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
    //cairo_set_source_rgb( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f  );
    cairo_fill( cr );
    
    //cairo_set_dash ( cr, dashes, 1, 0.0);
    cairo_set_line_width( cr, 1.0);
    
    // loop over each 2nd line, drawing dots
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, 0.4,0.4,0.4);
    for ( int i = 0; i < 2; i++ )
    {
      cairo_move_to( cr, x + ((w / 2.f)*i), y );
      cairo_line_to( cr, x + ((w / 2.f)*i), y + h );
    }
    for ( int i = 0; i < 4; i++ )
    {
      cairo_move_to( cr, x    , y + ((h / 4.f)*i) );
      cairo_line_to( cr, x + w, y + ((h / 4.f)*i) );
    }
    
    cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
    cairo_stroke(cr);
    //cairo_set_dash ( cr, dashes, 0, 0.0);
    
    
    // audio level
    if ( orientationHorizontal )
    {
      cairo_rectangle(cr, x, y+h*0.15, w * ampL, 9.9 );
      cairo_rectangle(cr, x, y+h*0.56, w * ampR, 9.9 );
    }
    else
    {
      cairo_rectangle(cr, x+w*0.15, y+h*(1-ampL), 9.9, h - h*(1-ampL) );
      cairo_rectangle(cr, x+w*0.56, y+h*(1-ampR), 9.9, h - h*(1-ampR) );
    }
    cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.21 );
    cairo_fill_preserve( cr );
    cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
    cairo_stroke(cr);
    
    // fader
    if ( orientationHorizontal )
      cairo_rectangle(cr, x+2+(w-24)*(value()), y+5, 20, h-10);
    else
      cairo_rectangle(cr, x+5, y+2+(h-24)*(1-value()), w-10, 20);
    
    cairo_set_source_rgba( cr,  1.0f, 0.48, 0.f, 1);
    cairo_set_line_width(cr, 1.9);
    cairo_stroke( cr );
    
    // stroke outline
    cairo_rectangle(cr, x, y, w, h);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_line_width(cr, 1.9);
    cairo_stroke( cr );
    
    cairo_restore( cr );
  }
}

void Volume::resize(int X, int Y, int W, int H)
{
  Fl_Widget::resize(X,Y,W,H);
  x = X;
  y = Y;
  w = W;
  h = H;
  redraw();
}

int Volume::handle(int event)
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
    case FL_MOUSEWHEEL:
      //printf("no scroll on faders\n");
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
