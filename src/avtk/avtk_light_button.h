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


#ifndef AVTK_LIGHT_BUTTON_H
#define AVTK_LIGHT_BUTTON_H

#include <FL/Fl_Button.H>

namespace Avtk
{

class LightButton : public Fl_Button
{
  public:
    LightButton(int _x, int _y, int _w, int _h, const char *_label):
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
    
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        if ( value() )
        {
          highlight = true;
        }
        else
        {
          highlight = false;
        }
        
        cairo_t *cr = Fl::cairo_cc();
        
        cairo_save( cr );
        
        cairo_rectangle( cr, x+1, y+1, w-2, h-2 );
        cairo_set_source_rgb( cr,28 / 255.f,  28 / 255.f ,  28 / 255.f  );
        cairo_fill_preserve(cr);
        
        cairo_set_line_width(cr, 1.5);
        cairo_rectangle( cr, x+1, y+1, w-2, h-2 );
        
        if ( highlight )
        {
          cairo_set_source_rgba(cr, 1.0, 0.48,   0, 0.4);
          cairo_fill_preserve(cr);
        }
        
        float alpha = 0.7;
        if (mouseOver)
          alpha = 1;
        cairo_set_source_rgba(cr, 1.0, 0.48,   0, alpha);
        cairo_stroke(cr);
        
        cairo_restore( cr );
        
        draw_label();
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
      switch(event) {
        case FL_ACTIVATE:
          {
          }
        case FL_DEACTIVATE:
          {
          }
          return 1;
        case FL_PUSH:
          highlight = 1;
          do_callback();
          return 1;
        case FL_DRAG: {
            int t = Fl::event_inside(this);
            if (t != highlight) {
              highlight = t;
              redraw();
            }
          }
          return 1;
        case FL_ENTER:
          mouseOver = true;
          redraw();
          return 1;
        case FL_LEAVE:
          mouseOver = false;
          redraw();
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

#endif // AVTK_LIGHT_BUTTON_H

