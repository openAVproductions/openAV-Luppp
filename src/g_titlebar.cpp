
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
  void TitleBar( Cairo::RefPtr<Cairo::Context> cr, float x, float y, float xS, float yS, std::string name, bool active)
  {
      
      if ( active )
        setColour(cr, COLOUR_GREEN_1 );
      else
        setColour(cr, COLOUR_GREY_1 );
      
      cr->rectangle(x,y, xS, 15);
      cr->set_line_width(2.8);
      cr->fill();
      
      cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size ( 13 );
      cr->move_to ( x + 8, y + 13 );
      cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
      
      cr->show_text ( name );
      //std::cout << "LupppWidget::TitleBar() called!" << std::endl;
  }
}
