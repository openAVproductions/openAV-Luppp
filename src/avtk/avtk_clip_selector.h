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

#include "../gridlogic.hxx"
#include "../gclipselectoraction.hxx"

#include "../worker.hxx"
#include "../looper.hxx"
#include "../audiobuffer.hxx"
#include "../eventhandler.hxx"


extern void luppp_tooltip(std::string s);

namespace Avtk
{

class ClipState
{
  public:
    ClipState()
    {
      _loaded = false;
      _playing = false;
      _queued = false;
      _recording = false;
      name = "";
    }
    
    void load(std::string n = "")
    {
      _loaded = true;
      name = n;
    }
    void play(){_playing = true;}
    void stop(){_playing = false; _recording = false;}
    void queue(){_queued = true;}
    void unqueue(){_queued = false;}
    void stopQueue(){_queued = false;}
    void record(){_recording = true; _loaded = true;}
    void stopRecord(){_recording = false;}
    
    bool loaded(){return _loaded;}
    bool playing(){return _playing;}
    bool queued() {return _queued;}
    bool recording(){return _recording;}
    std::string getName(){return name;}
  
  private:
    bool _loaded;
    bool _playing;
    bool _queued;
    bool _recording;
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
    
    
    int ID;
    
    static const int numClips = 10;
    ClipState clips[numClips];
    
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void setID( int id )
    {
      ID = id;
    }
    
    /** calling this function marks a clip as loaded, optionally with name
     parameter. Recording won't provide a name, or "..." or something
    **/
    void loadClip( int clip, std::string name = "" )
    {
      clips[clip].load(name);
    }
    
    /** converts the Looper::State into the UI represnted ClipSelector state.
     * It puts some of the data into clips[], and stores unique state into the class.
    **/
    void setState( int clipNum, GridLogic::State s )
    {
      
      switch(s)
      {
        case GridLogic::STATE_EMPTY:
            break;
        case GridLogic::STATE_LOADED:
            clips[clipNum].load();
            clips[clipNum].unqueue();
            //printf("clipSelector setState() clip %i = STATE_LOADED\n", clipNum);
            break;
        case GridLogic::STATE_PLAYING:
            //printf("clipSelector setState() clip %i = STATE_PLAYING\n", clipNum);
            for(int i = 0; i < numClips; i++ )
              clips[clipNum].stop();
            clips[clipNum].unqueue();
            clips[clipNum].play();
            break;
        case GridLogic::STATE_PLAY_QUEUED:
            for(int i = 0; i < numClips; i++ )
              clips[clipNum].unqueue();
            clips[clipNum].queue();
            //printf("clipSelector setState() clip %i = STATE_PLAY_QUEUED\n", clipNum);
            break;
        case GridLogic::STATE_RECORDING:
            for(int i = 0; i < numClips; i++ )
              clips[clipNum].stopRecord();
            clips[clipNum].unqueue();
            clips[clipNum].record();
            //printf("clipSelector setState() clip %i = STATE_RECORDING\n", clipNum);
            break;
        case GridLogic::STATE_RECORD_QUEUED:
            for(int i = 0; i < numClips; i++ )
              clips[clipNum].unqueue();
            clips[clipNum].queue();
            //printf("clipSelector setState() clip %i = STATE_RECORD_QUEUED\n", clipNum);
            break;
        /*
        case GridLogic::STATE_STOPPED:
            clips[clipNum].stop();
            printf("clipSelector setState() clip %i = STATE_STOPPED\n", clipNum);
            break;
        */
        case GridLogic::STATE_STOP_QUEUED:
            //printf("clipSelector setState() clip %i = STATE_STOP_QUEUED\n", clipNum);
            clips[clipNum].stopRecord();
            clips[clipNum].stop();
            clips[clipNum].queue();
            break;
      }
      
      redraw();
    }
    
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
          cairo_rectangle( cr, x+2, drawY, clipWidth-1, clipHeight - 4 );
          cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
          cairo_fill(cr);
          
          cairo_rectangle( cr, x+2, drawY, clipHeight-4, clipHeight - 4 );
          
          if ( clips[i].recording() )
          {
            cairo_set_source_rgba(cr, 1.f,  0 / 255.f ,  0 / 255.f, 1.f);
            cairo_fill(cr);
            cairo_arc( cr, x+14, drawY+13, 4.3, 0, 6.29 );
            cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
            cairo_fill(cr);
          }
          else if ( clips[i].playing() )
          {
            cairo_set_source_rgba(cr, 0.0, 1.0,   0, 1.f );
            cairo_fill(cr);
            cairo_move_to( cr, x+10, drawY+8 );
            cairo_line_to( cr, x+19, drawY+13 );
            cairo_line_to( cr, x+10, drawY+18 );
            cairo_close_path(cr);
            cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
            cairo_fill(cr);
          }
          /*
          else if ( i == stopQueuedClip )
          {
            cairo_set_source_rgba(cr, 0 / 255.f,  0 / 255.f ,  0 / 255.f, 1.0);
            cairo_fill(cr);
            cairo_rectangle( cr, x+9, drawY+8, 9, 9 );
            cairo_set_source_rgba(cr, 1, 1, 1, 0.6);
            cairo_fill(cr);
          }
          */
          else if ( clips[i].queued() )
          {
            cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
            cairo_fill(cr);
            cairo_move_to( cr, x+11, drawY+8  );
            cairo_line_to( cr, x+11, drawY+18 );
            cairo_move_to( cr, x+17, drawY+8  );
            cairo_line_to( cr, x+17, drawY+18 );
            cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
            cairo_set_line_width(cr, 2.8f);
            cairo_stroke(cr);
          }
          else if ( clips[i].loaded() )
          {
            cairo_set_source_rgba(cr, 1.0, 0.6,   0, 1.f);
            cairo_fill(cr);
            cairo_arc( cr, x+14, drawY+13, 4.3, 0, 6.29 );
            cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
            cairo_set_line_width(cr, 2.2f);
            cairo_stroke(cr);
          }
          else
          {
            cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 1.f);
            cairo_fill(cr);
          }
          
          cairo_rectangle( cr, x+2, drawY, clipWidth -1, clipHeight - 3 );
          
          float alpha = 1;
          cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, alpha);
          cairo_set_line_width( cr, 1.3);
          cairo_move_to( cr, x+clipHeight-1, drawY );
          cairo_line_to( cr, x+clipHeight-1, drawY + clipHeight - 2);
          cairo_stroke(cr);
          
          // clip name
          cairo_move_to( cr, x + clipHeight + 10, drawY + 16 );
          cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 1 );
          cairo_set_font_size( cr, 10 );
          cairo_show_text( cr, clips[i].getName().c_str() );
          
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
            
            // handle right clicks: popup menu
            if ( Fl::event_state(FL_BUTTON3) )
            {
              Fl_Menu_Item rclick_menu[] =
              {
                { "Load" },
                { "Bars",  0,   0, 0, FL_SUBMENU },
                  {"1"},
                  {"2"},
                  {"4"},
                  {"8"},
                  {"16"},
                  {"32"},
                  {"64"},
                  {0},
                //{ "Record" },
                { "Use as tempo" },
                { 0 }
              };
              Fl_Menu_Item *m = (Fl_Menu_Item*) rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
              if ( !m )
              {
                  return 0;
              }
              else if ( strcmp(m->label(), "Load") == 0 )
              {
                clipSelectorLoad( ID, clipNum );
                loadClip( clipNum, "---" );
              }
              else if ( strcmp(m->label(), "1") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,1);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "2") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,2);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "4") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,4);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "8") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,8);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "16") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,16);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "32") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,32);
                writeToDspRingbuffer( &e );
              } else if ( strcmp(m->label(), "64") == 0 ) {
                EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,64);
                writeToDspRingbuffer( &e );
              }
              else if ( strcmp(m->label(), "Use as tempo") == 0 )
              {
                EventLooperUseAsTempo e (ID, clipNum);
                writeToDspRingbuffer( &e );
              }
              else if ( strcmp(m->label(), "Record") == 0 )
              {
                /*
                //recordingClip = clipNum;
                EventLooperState e = EventLooperState( ID, clipNum, Looper::STATE_RECORD_QUEUED);
                writeToDspRingbuffer( &e );
                */
              }
            }
            else
            {
              // write "pressed" event for this track,scene
              EventGridEvent e( ID, clipNum, true );
              writeToDspRingbuffer( &e );
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
          // push help string to UI tooltip area
          luppp_tooltip( "Clip selector" );
          
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

