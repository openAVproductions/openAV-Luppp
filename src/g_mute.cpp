
#include "g_widgets.hpp"

#include <sstream>

namespace
{
  std::string floatToStr (float number)
  {
       std::ostringstream buff;
       buff<<number;
       return buff.str();
  }
}

namespace Luppp
{
  void Mute( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active)
  {
    //std::cout << "Mute() called, ID " << id << "  mode: " << mode << std::endl;
    // main rectangle
    if ( active )
      setColour(cr, COLOUR_GREEN_1 );
    else
      setColour(cr, COLOUR_GREY_1 );
    
    cr->move_to(x,y);
    cr->rectangle (x,y, 23, 23);
    cr->fill_preserve();
    
    // outline
    cr->set_line_width(1);
    setColour(cr, COLOUR_GREY_2 );
    cr->stroke();
    
    // id text
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 13 );
    cr->move_to ( x + 8, y + 17 );
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    
    std::string str = floatToStr(id);
    if ( id == -1 )
      str = "M";
    
    cr->show_text ( str );
  }
}
