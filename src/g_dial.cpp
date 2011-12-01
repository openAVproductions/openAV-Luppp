
#include "g_widgets.hpp"

namespace Luppp
{
  void Dial( Cairo::RefPtr<Cairo::Context> cr, bool active, float x, float y, float value, DialMode mode)
  {
      int xc = x + 16;
      int yc = y + 22;
      
      float radius = value * 20;
      
      cr->set_line_cap( Cairo::LINE_CAP_ROUND );
      cr->set_line_join( Cairo::LINE_JOIN_ROUND);
      
      // Arc Angle Value
      cr->set_line_width(2.4);
      cr->move_to(xc,yc);
      cr->set_source_rgba( 0,0,0,0 );
      cr->stroke();
      
      /*
      // main arc
      setColour(cr, COLOUR_GREY_4 );
      cr->arc(xc,yc, radius, 2.46, 0.75 );
      cr->move_to(xc,yc);
      cr->stroke();
      */
      
      cr->set_line_width(2.8);
      float angle;
      switch (mode)
      {
        case DIAL_MODE_NORMAL:
          //angle = 2.46 + (4.54 * value);
          
          setColour(cr, COLOUR_BLUE_1 );
          cr->move_to( xc - radius * 0.5, yc );
          cr->line_to( xc + radius * 0.5, yc );
          cr->move_to( xc, yc - (radius*0.5) );
          cr->line_to( xc, yc + (radius*0.5) );
          cr->stroke();
          
          if ( active )
            setColour(cr, COLOUR_ORANGE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
          cr->arc(xc,yc, radius, 0, 6.2830 );
          cr->line_to(xc,yc);
          cr->close_path();
          cr->fill();
          setColour(cr, COLOUR_GREY_1 );
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
          angle = -1.57 + (( value ) * (4.54 / 2.f) );
          if ( active )
            setColour(cr, COLOUR_ORANGE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
          
          if ( value > 0.f )
            cr->arc(xc,yc, 17, -1.57, angle );
          else
            cr->arc_negative(xc,yc, 17, -1.57, angle );
          cr->line_to(xc,yc);
          cr->close_path();
          
          // value check, only fill if we need to
          if ( ( value < -0.1 || value > 0.1 ) && active )
          {
            //cr->set_line_width(1.2);
            cr->fill_preserve();
          }
          
          setColour(cr, COLOUR_GREY_1 );
          cr->stroke();
          
          // draw triangle on top
          cr->set_line_width(1.2);
          float triangeX = x + 16;
          float triangeY = y + 8;
          if ( ( value < -0.1 || value > 0.1 ) && active )
            setColour(cr, COLOUR_ORANGE_1 );
          else
            setColour(cr, COLOUR_GREY_1 );
            
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
