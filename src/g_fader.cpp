
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
  void Fader(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float value, float rms, float falloff)
  {
    // draw invisible from the last widget
    cr->set_source_rgba(0,0,0,0);
    cr->move_to( x, y );
    cr->stroke();
    
    x = x + 4; // global move widget relative to positioning
    
    // Create the linear gradient diagonal
    //Cairo::RefPtr< Cairo::LinearGradient > linGrad = Cairo::LinearGradient::create (x, y, 100, 100);
    
    // Set grandient colors
    //linGrad->add_color_stop_rgb (0, 10 / 255.f, 246 / 255.f, 98 / 255.f );
    //linGrad->add_color_stop_rgb (1, 192 / 255.f, 246 / 255.f, 98 / 255.f );
    
    // Draw rectangle and fill with gradient
    cairo_pattern_t *pat;
    pat = cairo_pattern_create_linear (0.0, 0.0,  0.0, 400.0);
    cairo_pattern_add_color_stop_rgb(pat, 0,   0 / 255.f, 153 / 255.f, 255 / 255.f );
    cairo_pattern_add_color_stop_rgb(pat, 1,  26 / 255.f,  26 / 255.f,  26 / 255.f );
    cr->rectangle   (x, y, 12, 94);
    cairo_set_source(cr->cobj(), pat);
    cairo_fill      (cr->cobj());
    cairo_pattern_destroy (pat);
    
    // red area on top of fader graphic
    cr->rectangle ( x, y, 12, 25 );
    setColour(cr, COLOUR_ORANGE_1 );
    cr->fill();
    
    float tmpRms = (1-rms);
    
    //std::cout << "Falloff: " << falloff << " RMS " << rms << std::endl;
    
    if ( falloff > rms )
    {
      //std::cout << "Falloff > RMS" << std::endl;
      tmpRms = (1-falloff);
      falloff -= 0.1;
    }
    else
    {
      falloff = rms;
    }
    
    //std::cout << " tmpRms " << tmpRms << "  drawPx:  " << tmpRms * 94 << std::endl;
    
    cr->rectangle ( x, y, 12, 94 * tmpRms );
    setColour(cr, COLOUR_GREY_2 );
    cr->fill();
    
    // draw fader  <|
    float playheadX = x + 12;
    float playheadY = y + (92 * ( 1.f - value));
    
    setColour(cr, COLOUR_ORANGE_1 );
    cr->set_line_width(0.8);
    cr->move_to( playheadX, playheadY );
    cr->line_to( playheadX + 10, playheadY + 5.5 );
    cr->line_to( playheadX + 10, playheadY - 5.5 );
    cr->close_path();
    cr->fill_preserve();
    
    // draw fader |>
    cr->move_to( x, playheadY );
    cr->line_to( x - 10, playheadY + 5.5 );
    cr->line_to( x - 10, playheadY - 5.5 );
    cr->close_path();
    cr->fill_preserve();
    cr->stroke();
    
    // line between fader markers |> --- <|
    setColour(cr, COLOUR_GREY_4 );
    cr->set_line_width(2);
    cr->move_to( x, playheadY );
    cr->line_to( x + 12, playheadY );
    cr->stroke();
  }
}
