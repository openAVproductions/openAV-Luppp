
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
  void Dial( Cairo::RefPtr<Cairo::Context> cr, bool active, float x, float y, float value, DialMode mode)
  {
      int xc = x + 16;
      int yc = y + 22;
      
      float radius = 14;
      
      cr->set_line_cap( Cairo::LINE_CAP_ROUND );
      cr->set_line_join( Cairo::LINE_JOIN_ROUND);
      cr->set_line_width(2.8);
      
      // Arc Angle Value
      cr->set_line_width(2.4);
      cr->move_to(xc,yc);
      cr->set_source_rgba( 0,0,0,0 );
      cr->stroke();
      
      // main arc
      if ( active )
        setColour(cr, COLOUR_GREY_4 );
      else
        setColour(cr, COLOUR_GREY_3 );
      cr->arc(xc,yc, radius, 2.46, 0.75 );
      cr->move_to(xc,yc);
      cr->stroke();
      
      cr->set_line_width(2.8);
      float angle;
      switch (mode)
      {
        case DIAL_MODE_NORMAL:
          if ( value < 0 )
            value = 0.f;
          //std::cout << "Dial NORMAL: value = " << value << std::endl;
          angle = 2.46 + (4.54 * value);
          if ( active )
            setColour(cr, COLOUR_ORANGE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
          cr->set_line_width(1.7);
          cr->arc(xc,yc, 13, 2.46, angle );
          cr->line_to(xc,yc);
          cr->stroke();
          cr->arc(xc,yc, 17, 2.46, angle );
          cr->line_to(xc,yc);
          cr->stroke();
          break;
        case DIAL_MODE_SEND:
          angle = 2.46 + (4.54 * value);
          if ( active )
            setColour(cr, COLOUR_BLUE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
          cr->arc(xc,yc, radius, 2.46, angle );
          cr->line_to(xc,yc);
          cr->stroke();
          break;
        case DIAL_MODE_PAN:
          // calculate "angle" from value for PAN style display
          angle = -1.57 + (( value ) * (4.54 / 2.f) );
          
          // colour & draw arcs for value rings
          if ( active )
            setColour(cr, COLOUR_ORANGE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
          cr->set_line_width(1.7);
          
          // outer "value" ring
          if ( value > 0.f )
            cr->arc(xc,yc, 17, -1.57, angle );
          else
            cr->arc_negative(xc,yc, 17, -1.57, angle );
          cr->line_to(xc,yc);
          cr->move_to(xc,yc);
          cr->stroke(); // stroke rings seperatly
          
          // inner "value" ring
          if ( value > 0.f )
            cr->arc(xc,yc, 13, -1.57, angle );
          else
            cr->arc_negative(xc,yc, 13, -1.57, angle );
          cr->stroke();
          
          
          // draw triangle on top
          cr->set_line_width(1.2);
          float triangeX = x + 16;
          float triangeY = y + 8;
          if ( ( value < -0.1 || value > 0.1 ) && !active )
            setColour(cr, COLOUR_GREY_4 );
          else
            setColour(cr, COLOUR_ORANGE_1 );
            
          cr->move_to( triangeX, triangeY );
          cr->line_to( triangeX - 5.5, triangeY - 10 );
          cr->line_to( triangeX + 5.5, triangeY - 10 );
          cr->close_path();
          cr->fill_preserve();
          
          setColour(cr, COLOUR_GREY_3 );
          cr->stroke();
          
          break;
      }
      
      //std::cout << "LupppWidget::Dial() called!" << std::endl;
  }
}
