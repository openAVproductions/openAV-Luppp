
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
