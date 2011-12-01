
#include "g_widgets.hpp"

namespace Luppp
{
  void GridBlock( Cairo::RefPtr<Cairo::Context> cr, float x, float y, BlockState state)
  {
    int blockSize = 17;
    
    if      ( state == BLOCK_STATE_EMPTY )
      setColour(cr, COLOUR_GREY_3 );
    else if ( state == BLOCK_STATE_LOADED )
      setColour(cr, COLOUR_PURPLE_3);
    
    cr->set_line_width(0.9);
    cr->rectangle (x,y + blockSize, 74, blockSize);
    cr->fill_preserve();
    
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0 /255.f );
    cr->stroke();
    
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 12 );
    cr->move_to ( x + 8, (y + 14) + blockSize );
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    cr->show_text ( "name" );
  }
}
