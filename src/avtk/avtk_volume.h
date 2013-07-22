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


#ifndef AVTK_VOLUME_H
#define AVTK_VOLUME_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Slider.H>
#include <valarray>
#include <string>

namespace Avtk
{
  
class Volume : public Fl_Slider
{
  public:
    Volume(int _x, int _y, int _w, int _h, const char *_label = 0):
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
    }
    
    void amplitude  (float aL, float aR) {ampL = aL; ampR = aR; redraw();}
    void compression(float c) {compress = c; redraw();}
    
    bool active;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    int mouseClickedX;
    int mouseClickedY;
    bool mouseClicked;
    
    float ampL, ampR;
    float compress;
    
    void set_active(bool a)
    {
      active = a;
      redraw();
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
        cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
        //cairo_set_source_rgb( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f  );
        cairo_fill( cr );
        
        
        // set up dashed lines, 1 px off, 1 px on
        double dashes[1];
        dashes[0] = 2.0;
        
        cairo_set_dash ( cr, dashes, 1, 0.0);
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
        cairo_set_dash ( cr, dashes, 0, 0.0);
        
        
        // audio level
        
        //cairo_move_to(cr, x+w*0.3, y+h); // left
        //cairo_line_to(cr, x+w*0.3, y+h*(1-ampL) );
        
        //cairo_move_to(cr, x+w*0.7, y+h); // right
        //cairo_line_to(cr, x+w*0.7, y+h*(1-ampL) );
        
        
        cairo_rectangle(cr, x+w*0.15, y+h*(1-ampL), 9.9, h - h*(1-ampL) );
        cairo_rectangle(cr, x+w*0.56, y+h*(1-ampR), 9.9, h - h*(1-ampR) );
        
        cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.21 );
        cairo_fill_preserve( cr );
        cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
        cairo_stroke(cr);
        
        // compression
        //cairo_rectangle(cr, x+w*0.45, y+h - h*(1-ampL), 8.9, h*(1-ampL));
        
        // fader
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
              
              float deltaY = mouseClickedY - Fl::event_y();
              
              float valY = value();
              valY += deltaY / h;
              
              if ( valY > 1.0 ) valY = 1.0;
              if ( valY < 0.0 ) valY = 0.0;
              
              //handle_drag( value + deltaY );
              set_value( valY );
              
              mouseClickedX = Fl::event_x();
              mouseClickedY = Fl::event_y();
              redraw();
              do_callback();
            }
          }
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
    
  private:
};

} // Avtk

#endif // AVTK_VOLUME_H
