
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "g_widgets.hpp"

namespace Luppp
{
  void Solo( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active)
  {
    //std::cout << "Solo() called, ID " << id << "  mode: " << mode << std::endl;
    // main rectangle
    if ( active )
      setColour(cr, COLOUR_BLUE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
      
    cr->move_to(x,y);
    cr->rectangle (x,y, 23, 13);
    cr->fill_preserve();
    
    // outline
    cr->set_line_width(1);
    cr->set_source_rgb( 85 / 255.f, 93/255.f , 99/255.f );
    cr->stroke();
    
    // id text
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    cr->set_font_size ( 11 );
    cr->move_to ( x + 1, y + 11 );
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    
    cr->show_text ( "PFL" );
  }
}
