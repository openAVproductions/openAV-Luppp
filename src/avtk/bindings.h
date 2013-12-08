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

#ifndef AVTK_BINDINGS_H
#define AVTK_BINDINGS_H

#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include "../controller/binding.hxx"

namespace Avtk
{

class Bindings : public Fl_Button
{
  public:
    Bindings( int _x, int _y, int _w, int _h, const char *_label = 0 );
    
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void add( Binding* b );
    
    void draw();
    //int handle(int event);
    void resize(int X, int Y, int W, int H);
    
    int bindYPx;
    std::vector<Binding*> binds;
    
};

}

#endif // AVTK_BINDINGS_H
