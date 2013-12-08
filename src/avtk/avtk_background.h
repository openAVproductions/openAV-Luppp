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


#ifndef AVTK_BACKGROUND_H
#define AVTK_BACKGROUND_H

#include <FL/Fl_Widget.H>
#include <FL/fl_ask.H>

#include <valarray>
#include <string>

namespace Avtk
{

// FIXME: Refactor to .hxx .cxx
class Background : public Fl_Widget
{
  public:
    Background(int _x, int _y, int _w, int _h, const char *_label = 0):
        Fl_Widget(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      label = strdup(_label);
      
      highlight = false;
    }
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void setLabel(const char* l)
    {
      if( label )
        free( (char*) label);
      
      label = strdup( l );
      redraw();
    }
    const char* getLabel()
    {
      return label;
    }
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        cairo_save( cr );
        
        cairo_set_line_width(cr, 1.5);
        
        
        // fill background
        cairo_rectangle( cr, x, y, w, h);
        cairo_set_source_rgba( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f , 1 );
        cairo_fill( cr );
        
        // draw header
          // backing
          cairo_rectangle(cr, x, y, w, 20);
          cairo_set_source_rgb( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f );
          cairo_fill( cr );
          
          // text
          cairo_move_to( cr, x + 10, y + 14 );
          cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
          cairo_set_font_size( cr, 10 );
          cairo_show_text( cr, label );
          
          // lower stripe
          cairo_move_to( cr, x    , y + 20 );
          cairo_line_to( cr, x + w, y + 20 );
          cairo_stroke( cr );
        
        
        // stroke rim
        cairo_rectangle(cr, x, y, w, h);
        cairo_set_source_rgba( cr, 66 / 255.f, 66 / 255.f , 66 / 255.f , 1 );
        cairo_stroke( cr );
        
        
        cairo_restore( cr );
      }
    }
    
    void resize(int X, int Y, int W, int H)
    {
      Fl_Widget::resize(X,Y,W,H);
      x = X;
      y = Y;
      w = W;
      h = H;
      redraw();
    }
    
    int handle(int event)
    {
      switch(event)
      {
        case FL_PUSH:
          if ( Fl::event_state(FL_BUTTON3) && Fl::event_y() < y + 20 )
          {
            const char* name = fl_input( "Track name: ", "" );
            if ( name )
            {
              free( (char*) label );
              label = strdup( name );
              redraw();
            }
            return 1;
          }
          
          redraw();
          return 1;
        case FL_DRAG: {
            int t = Fl::event_inside(this);
            if (t != highlight) {
              redraw();
            }
          }
          return 1;
        case FL_RELEASE:
          if (highlight) {
            highlight = 0;
            redraw();
            do_callback();
          }
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
};

} // Avtk

#endif // AVTK_BACKGROUND_H

