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


#ifndef AVTK_VOLUME_H
#define AVTK_VOLUME_H

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Slider.H>
#include <valarray>
#include <string>

namespace Avtk
{
  class Volume : public Fl_Slider
  {
    public:
      Volume(int _x, int _y, int _w, int _h, const char *_label = 0);
      
      void set_active(bool a);
      void setOrientationHorizontal();
      
      void fader( float f );
      void compression(float c);
      void amplitude  (float aL, float aR);
      
      void draw();
      int  handle(int event);
      
      void resize(int X, int Y, int W, int H);
      
      
    private:
      bool active;
      bool highlight;
      bool orientationHorizontal;
      
      int x, y, w, h;
      const char* label;
      
      int mouseClickedX;
      int mouseClickedY;
      bool mouseClicked;
      
      float ampL, ampR;
      float compress;
  };
} // Avtk

#endif // AVTK_VOLUME_H
