
#include "g_widgets.hpp"

namespace Luppp
{
  void Block( Cairo::RefPtr<Cairo::Context> cr, float x, float y, BlockState state)
  {
    int blockSize = 17;
    
    Colour textColour, playColour, bgColour;
    
    if        ( state == BLOCK_STATE_EMPTY ) {
     textColour = COLOUR_TRANSPARENT;
     playColour = COLOUR_TRANSPARENT;
     bgColour   = COLOUR_GREY_4;
    } else if ( state == BLOCK_STATE_LOADED ){
     textColour = COLOUR_GREY_1;
     playColour = COLOUR_TRANSPARENT;
     bgColour   = COLOUR_GREY_3;
    } else if ( state == BLOCK_STATE_PLAYING ){
     textColour = COLOUR_GREY_4;
     playColour = COLOUR_GREY_4;
     bgColour   = COLOUR_GREEN_1;
    }
    
    setColour(cr, bgColour);
    cr->set_line_width(0.9);
    cr->rectangle (x,y, 74, blockSize);
    cr->fill();
    
    // draw "play square" on left of block
    setColour(cr, playColour);
    cr->rectangle (x+1, y+1, 15, 15);
    cr->fill();
    
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 12 );
    cr->move_to ( x + 22, (y + 13) );
    setColour(cr, textColour);
    cr->show_text ( "name" );
  }
}
