
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
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 13 );
    cr->move_to ( x + 8, y + 11 );
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    
    cr->show_text ( "s" );
  }
}
