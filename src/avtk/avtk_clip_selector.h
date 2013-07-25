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


#ifndef AVTK_CLIP_SELECTOR_H
#define AVTK_CLIP_SELECTOR_H

#include <FL/Fl_Button.H>

#include <string>

#include "../gclipselectoraction.hxx"

namespace Avtk
{

class ClipState
{
  public:
    enum State {
      CLIP_EMPTY = 0,
      CLIP_LOADED,
      CLIP_QUEUED,
      CLIP_PLAYING,
      CLIP_RECORDING,
      CLIP_STOPPING,
    };
    ClipState()
    {
      state = CLIP_EMPTY;
      name = "";
    }
    ClipState(std::string n)
    {
      state = CLIP_EMPTY;
      name = n;
    }
    
    State state;
    std::string name;
};

class ClipSelector : public Fl_Button
{
  public:
    
    
    ClipSelector(int _x, int _y, int _w, int _h, const char *_label):
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
    
    static const int numClips = 10;
    ClipState clips[numClips];
    
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
        
        int clipWidth  = w - 2;
        int clipHeight = (h / numClips);
        
        int drawY = y + 1;
        for( int i = 0; i < numClips; i++) // draw each clip
        {
          cairo_rectangle( cr, x+1, drawY, clipWidth, clipHeight - 4 );
          cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
          cairo_fill(cr);
          
          cairo_rectangle( cr, x+1, drawY, clipHeight - 2, clipHeight - 4 );
          switch( clips[i].state )
          {
            case ClipState::CLIP_EMPTY:
                cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 1.f);
                cairo_fill(cr);
                break;
            case ClipState::CLIP_LOADED:
                cairo_set_source_rgba(cr, 1.0, 0.6,   0, 1.f);
                cairo_fill(cr);
                cairo_arc( cr, x+14, drawY+13, 4.3, 0, 6.29 );
                cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
                cairo_set_line_width(cr, 2.2f);
                cairo_stroke(cr);
                break;
            case ClipState::CLIP_QUEUED:
                cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
                cairo_fill(cr);
                cairo_move_to( cr, x+11, drawY+8  );
                cairo_line_to( cr, x+11, drawY+18 );
                cairo_move_to( cr, x+17, drawY+8  );
                cairo_line_to( cr, x+17, drawY+18 );
                cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
                cairo_set_line_width(cr, 2.8f);
                cairo_stroke(cr);
                break;
            case ClipState::CLIP_PLAYING:
                cairo_set_source_rgba(cr, 0.0, 1.0,   0, 1.f );
                cairo_fill(cr);
                cairo_move_to( cr, x+10, drawY+8 );
                cairo_line_to( cr, x+19, drawY+13 );
                cairo_line_to( cr, x+10, drawY+18 );
                cairo_close_path(cr);
                cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
                cairo_fill(cr);
                break;
            case ClipState::CLIP_RECORDING:
                cairo_set_source_rgba(cr, 1.f,  0 / 255.f ,  0 / 255.f, 1.f);
                cairo_fill(cr);
                cairo_arc( cr, x+14, drawY+13, 4.3, 0, 6.29 );
                cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
                cairo_fill(cr);
                break;
            case ClipState::CLIP_STOPPING:
                cairo_set_source_rgba(cr, 0 / 255.f,  0 / 255.f ,  0 / 255.f, 1.0);
                cairo_fill(cr);
                cairo_rectangle( cr, x+9, drawY+8, 9, 9 );
                cairo_set_source_rgba(cr, 1, 1, 1, 0.6);
                cairo_fill(cr);
                break;
          }
          
          
          
          
          
          cairo_rectangle( cr, x+1, drawY, clipWidth, clipHeight - 2 );
          
          /*
          //cairo_set_source_rgb( cr,28 / 255.f,  28 / 255.f ,  28 / 255.f  );
          if ( highlight )
          {
            cairo_set_source_rgba(cr, 1.0, 0.48,   0, 0.4);
            cairo_fill_preserve(cr);
          }
          */
          
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
          cairo_show_text( cr, clips[i].name.c_str() );
          
          drawY += clipHeight;
        }
        
        cairo_restore( cr );
        
        //draw_label();
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
          {
            // calculate the clicked clip number
            int clipHeight = (h / numClips);
            int clipNum = ( (Fl::event_y() ) - y ) / clipHeight;
            if (clipNum >= numClips)
              clipNum = numClips -1; // fix for clicking the lowest pixel
            printf("clip number %i\n" , clipNum);
            
            
            // handle right clicks: popup menu
            if ( Fl::event_state(FL_BUTTON3) )
            {
              Fl_Menu_Item rclick_menu[] =
              {
                { "Load" },
                { "Duration" },
                { 0 }
              };
              Fl_Menu_Item *m = (Fl_Menu_Item*) rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
              if ( !m )
              {
                  return 0;
              }
              else if ( strcmp(m->label(), "Load") == 0 )
              {
                clipSelectorLoad( 0 );
                clips[clipNum].state = ClipState::CLIP_LOADED;
              }
              else if ( strcmp(m->label(), "Duration") == 0 )
              {
                //clips[clipNum].name = "title";
                clips[clipNum].state = ClipState::CLIP_QUEUED;
              }
            }
            else
            {
              switch( clips[clipNum].state )
              {
                case ClipState::CLIP_EMPTY:
                    clips[clipNum].state = ClipState::CLIP_RECORDING;
                    break;
                case ClipState::CLIP_LOADED:
                      clips[clipNum].state = ClipState::CLIP_QUEUED;
                    break;
                case ClipState::CLIP_QUEUED:
                    clips[clipNum].state = ClipState::CLIP_PLAYING;
                    break;
                case ClipState::CLIP_PLAYING:
                    clips[clipNum].state = ClipState::CLIP_STOPPING;
                    break;
                case ClipState::CLIP_RECORDING:
                    clips[clipNum].state = ClipState::CLIP_STOPPING;
                    break;
                case ClipState::CLIP_STOPPING:
                    clips[clipNum].state = ClipState::CLIP_PLAYING;
                    break;
                default:
                    printf("Avtk::ClipSelector, warning: unknown clip type\n");
                
              }
            }
          }
          redraw();
          do_callback();
          return 1;
        case FL_DRAG:
          {
            int t = Fl::event_inside(this);
            if (t != highlight)
            {
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

#endif // AVTK_CLIP_SELECTOR_H

