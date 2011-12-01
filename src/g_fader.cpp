
#include "g_widgets.hpp"

namespace Luppp
{
  void Fader(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float value, float rms, float peak)
  {
    // draw invisible from the last widget
    cr->set_source_rgba(0,0,0,0);
    cr->move_to( x, y );
    cr->stroke();
    
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
    
    // draw fader  <|
    float playheadX = x + 12;
    float playheadY = y + (92 * ( 1.f - value));
    
    setColour(cr, COLOUR_ORANGE_1 );
    cr->set_line_width(1.0);
    cr->move_to( playheadX, playheadY );
    cr->line_to( playheadX + 10, playheadY + 5.5 );
    cr->line_to( playheadX + 10, playheadY - 5.5 );
    cr->close_path();
    cr->fill_preserve();
    
    setColour(cr, COLOUR_GREY_1 );
    cr->stroke();
    
  }
}
