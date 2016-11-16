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


#ifndef AVTK_IMAGE_H
#define AVTK_IMAGE_H

#include <FL/Fl_Widget.H>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

#include <FL/fl_draw.H>

#include "../gui.hxx"
extern Gui* gui;

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
      
      bits = -1;
      imageDataPtr = 0;
      
      stickToRight = false;
    }
    
    // used to keep images on the right hand side of the window
    bool stickToRight;
    
    void setPixbuf(const unsigned char* data, int b )
    {
      bits = b;
      imageDataPtr = data;
    }
    
    int x, y, w, h;
    const char* label;
    
    int bits;
    const unsigned char* imageDataPtr;
    
    void draw()
    {
      if ( damage() & FL_DAMAGE_ALL && imageDataPtr != 0 )
      {
        fl_draw_image((const uchar*)imageDataPtr, x, y, w, h, bits, w*bits);
      }
    }
    
    void resize(int X, int Y, int W, int H)
    {
      if ( stickToRight )
      {
        x = gui->getWindowWidth() - w;
      }
      
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

