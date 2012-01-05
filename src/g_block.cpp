
#include "g_widgets.hpp"

#include "clipselectorstate.hpp"

#include <iostream>

namespace Luppp
{
  void Block( Cairo::RefPtr<Cairo::Context> cr, float x, float y, ClipState state, std::string name)
  {
    int blockSize = 17;
    
    Colour textColour, playColour, bgColour, recColour;
    
    recColour = COLOUR_TRANSPARENT;
    
    if        ( state == CLIP_STATE_EMPTY ) {
     textColour = COLOUR_TRANSPARENT;
     playColour = COLOUR_TRANSPARENT;
     recColour  = COLOUR_ORANGE_1;
     bgColour   = COLOUR_GREY_4;
    } else if ( state == CLIP_STATE_LOADED ){
     textColour = COLOUR_BLUE_1;
     playColour = COLOUR_GREY_3;
     bgColour   = COLOUR_ORANGE_1;
    } else if ( state == CLIP_STATE_PLAYING ){
     textColour = COLOUR_GREY_4;
     playColour = COLOUR_GREY_4;
     bgColour   = COLOUR_GREEN_1;
    } else if ( state == CLIP_STATE_RECORDING ){
     std::cout << "Doing a RECORD BLOCK NOW!" << std::endl;
     textColour = COLOUR_TRANSPARENT;
     playColour = COLOUR_GREY_4;
     bgColour   = COLOUR_RECORD_RED;
    }
    
    setColour(cr, bgColour);
    cr->set_line_width(0.9);
    cr->rectangle (x,y, 74, blockSize);
    cr->fill();
    
    // draw "play square" on left of block
    setColour(cr, playColour);
    cr->rectangle (x+1, y+1, 15, 15);
    cr->fill();
    
    if ( recColour != COLOUR_TRANSPARENT )
    {
        // draw "record circle" on left of block
        cr->arc(x+8, y+8, 4, 0, 6.28 );
        setColour(cr, recColour);
        cr->set_line_width(2.2);
        cr->stroke();
    }
    
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 13 );
    cr->move_to ( x + 22, (y + 13) );
    setColour(cr, textColour);
    cr->show_text ( name );
  }
}
