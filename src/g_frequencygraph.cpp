
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

#include <vector>

namespace Luppp
{
  void FrequencyGraph( Cairo::RefPtr<Cairo::Context> cr, bool active, float x, float y, float xS, float yS, EqualizerState state)
  {
      cr->set_line_cap( Cairo::LINE_CAP_ROUND );
      cr->set_line_join( Cairo::LINE_JOIN_ROUND);
      
      int xSize = xS;
      int ySize = yS;
      
      // works but a bit simple
      cr -> move_to( x        , y         );
      cr -> line_to( x + xSize, y         );
      cr -> line_to( x + xSize, y + ySize );
      cr -> line_to( x        , y + ySize );
      cr -> close_path();
      
      // Draw outline shape
      cr -> set_source_rgb (0.1,0.1,0.1);
      cr -> fill();
      
      // draw "frequency guides"
      std::valarray< double > dashes(2);
      dashes[0] = 2.0;
      dashes[1] = 2.0;
      cr->set_dash (dashes, 0.0);
      cr->set_line_width(1.0);
      cr->set_source_rgb (0.4,0.4,0.4);
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x + ((xSize / 4.f)*i), y );
        cr->line_to( x + ((xSize / 4.f)*i), y + ySize );
      }
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x       , y + ((ySize / 4.f)*i) );
        cr->line_to( x +xSize, y + ((ySize / 4.f)*i) );
      }
      cr->stroke();
      cr->unset_dash();
      
      // set colour based on active or not
      if ( active )
        setColour(cr, COLOUR_BLUE_1, 0.2 );
      else
        setColour(cr, COLOUR_GREY_1, 0.2 );
      
      int tmpX = x;
      int tmpY = y;
      
      // precalculate some variables
      float oldGainPix = (ySize / 60.f) * (state.gain[0] - 0.5 ) * 40;
      float oldXLoc = 0;
      float qPix = ((xSize * 0.2) / 3.f );
      //float oldCutoff = 0;
      
      // move to bottom left, draw line to middle left
      cr->move_to( tmpX, tmpY + ySize         );
      cr->line_to( tmpX, tmpY + (ySize * 0.5) - oldGainPix );
      
      
      for ( int i = 0; i < 4; i++ )
      {
        //float cutoff = state.cutoffFreq[i] / 20000;
        
        
        float gainPix = (ySize / 60.f) * (state.gain[i] - 0.5 ) * 40;
        
        float xLoc = xSize * 0.2 * (i+1);
        
        //std::cout << "I: " << i << "  GainPix: " << gainPix << "  tmpY - gainPix" << tmpY - gainPix << std::endl;
        
        
        cr->curve_to( tmpX + oldXLoc + qPix, tmpY + (ySize * 0.5) - oldGainPix ,// control point 1
                      tmpX + xLoc - qPix   , tmpY + (ySize * 0.5) - gainPix ,   // control point 2
                      tmpX + xLoc          , tmpY + (ySize * 0.5) - gainPix );  // end of curve
        
        // update variables for next iter
        oldGainPix = gainPix;
        oldXLoc = xLoc;
        //oldCutoff = cutoff;
      }
      
      // last bit of curve to the right edge
      cr->curve_to( tmpX + oldXLoc + qPix, tmpY + (ySize * 0.5) - oldGainPix,   // control point 1
                    tmpX + xSize   - qPix, tmpY + (ySize * 0.5) - oldGainPix,   // control point 2
                    tmpX + xSize         , tmpY + (ySize * 0.5) - oldGainPix);  // end of curve
      
      
      cr->line_to( tmpX + xSize , tmpY + ySize );
      cr->close_path();
      cr->fill_preserve();
      
      cr->set_line_width(2.5);
      if ( active )
        setColour(cr, COLOUR_BLUE_1 );
      else
        setColour(cr, COLOUR_GREY_1 );
      cr->stroke();
      
      // outline
      cr->rectangle( x, y , xS, yS );
      cr->set_line_width(3);
      if ( active )
        setColour(cr, COLOUR_GREY_2 );
      else
        setColour(cr, COLOUR_GREY_3 );
      cr->stroke();
      
      //std::cout << "LupppWidget::FrequencyGraph() called!" << std::endl;
  }
}
