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


#ifndef AVTK_UNIT_H
#define AVTK_UNIT_H

#include <FL/Fl_Button.H>

#include <string>

namespace Avtk
{

class UnitState
{
  public:
    enum State {
      UNIT_EMPTY = 0,
      UNIT_LOADED,
      UNIT_QUEUED,
      UNIT_PLAYING,
      UNIT_RECORDING,
      UNIT_STOPPING,
    };
    UnitState()
    {
      state = UNIT_EMPTY;
      name = "Clip";
    }
    UnitState(std::string n)
    {
      state = UNIT_EMPTY;
      name = n;
    }
  
    State state;
    std::string name;
};

class Unit : public Fl_Button
{
  public:
    Unit(int _x, int _y, int _w, int _h, const char *_label):
        Fl_Button(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      label = _label;
      
      highlight = false;
      mouseOver = false;
      
      clips[0].state = UnitState::UNIT_EMPTY;
      clips[1].state = UnitState::UNIT_LOADED;
      clips[2].state = UnitState::UNIT_QUEUED;
      clips[3].state = UnitState::UNIT_PLAYING;
      clips[4].state = UnitState::UNIT_RECORDING;
      clips[5].state = UnitState::UNIT_STOPPING;
    }
    
    static const int numClips = 10;
    UnitState clips[numClips];
    
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
        
        cairo_rectangle( cr, x+2, y + 2, w - 2, h - 2 );
        cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
        cairo_fill(cr);
        
        //cairo_rectangle( cr, x+1, drawY, clipHeight - 2, clipHeight - 2 );
        /*
        cairo_rectangle( cr, x+1, drawY, clipWidth, clipHeight - 2 );
        
        float alpha = 0.7;
        if (mouseOver) { alpha = 1; }
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, alpha);
        cairo_set_line_width( cr, 1.4);
        cairo_move_to( cr, x+clipHeight-1, drawY );
        cairo_line_to( cr, x+clipHeight-1, drawY + clipHeight - 2);
        cairo_stroke(cr);
        
        // clip name
        cairo_move_to( cr, x+clipHeight-1+ 10, drawY + 15 );
        cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 1 );
        cairo_set_font_size( cr, 10 );
        cairo_show_text( cr, label );
        */
        
        // outline
        float alpha = 0.7;
        if (mouseOver) { alpha = 1; }
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, alpha);
        cairo_rectangle( cr, x+2, y + 2, w - 2, h - 2 );
        cairo_set_line_width( cr, 1.4);
        cairo_stroke( cr );
        
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

#endif // AVTK_UNIT_H

