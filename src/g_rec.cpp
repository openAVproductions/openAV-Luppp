
#include "g_widgets.hpp"

namespace Luppp
{
  void Rec( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active)
  {
    //std::cout << "Rec() called, ID " << id << "  mode: " << mode << std::endl;
    // main rectangle
    if ( active )
      setColour(cr, COLOUR_PURPLE_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    
    cr->move_to(x,y);
    cr->rectangle (x,y, 23, 13);
    cr->fill_preserve();
    
    // outline
    cr->set_line_width(1);
    cr->set_source_rgb( 85 / 255.f, 93/255.f , 99/255.f );
    cr->stroke();
    
    // record dot
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    cr->move_to(x + 11,y + 6.5);
    cr->arc(x + 11.5, y  + 6.5, 4, 0, 6.28 );
    cr->fill();
  }
}
