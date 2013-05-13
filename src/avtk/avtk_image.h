/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */


#ifndef AVTK_IMAGE_H
#define AVTK_IMAGE_H

#include <FL/Fl_Widget.H>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Avtk
{

class Image : public Fl_Widget
{
  public:
    Image(int _x, int _y, int _w, int _h, const char *_label=0 ):
        Fl_Widget(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      label = _label;
      
      imageSurf = 0;
      
      if ( _label )
      {
        //cout << "creating image from label" << endl;
        imageSurf = cairo_image_surface_create_from_png( label );
      }
    }
    
    int x, y, w, h;
    const char* label;
    
    cairo_surface_t*  imageSurf;
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        //cairo_save(cr);
        
        if ( imageSurf == 0 )
        {
          // draw X
          cairo_move_to( cr,  x    , y     );
          cairo_line_to( cr,  x + w, y + h );
          cairo_move_to( cr,  x    , y + h );
          cairo_line_to( cr,  x + w, y     );
          cairo_set_source_rgb ( cr, 0.2,0.2,0.2);
          cairo_stroke(cr);
          
          // draw text
          cairo_move_to( cr,  x + (w/2.f) - 65, y + (h/2.f) + 10 );
          cairo_set_source_rgb ( cr, 0.6,0.6,0.6);
          cairo_set_font_size( cr, 20 );
          cairo_show_text( cr, "Image not loaded" );
          
          return;
        }
        
        // draw the image to the context
        cairo_set_source_surface(cr, imageSurf, x, y);
        //cairo_rectangle( cr, x, y, w, h );
        cairo_paint(cr);
        
        //cairo_restore(cr);
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
          do_callback();
          return 1;
        case FL_DRAG:
          return 1;
        case FL_RELEASE:
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

#endif

