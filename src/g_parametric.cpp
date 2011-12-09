
#include "g_widgets.hpp"

namespace Luppp
{
  void EQ(Cairo::RefPtr<Cairo::Context> cr, bool selected, bool active, float x, float y, float xS, float yS,  FrequencyGraphState freqState )
  {
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle(x, y, xS, yS);
    cr->fill();
    
    int dialSize = 30;
    
    // setup array of dials
    for ( int i = 0; i < 4; i++ )
    {
      for( int j = 0; j < 2; j++ )
      {
        float value = 0.f;
        if ( j % 2 == 0 )
          value = (freqState.gain[i] * 2) - 1;
        else
        {
          value = (freqState.cutoffFreq[i] - 0.5)*2;
        }
        Dial(cr, active, x + 30 + ( 50 * i)  , y + 130 + 40 * j , value , DIAL_MODE_PAN);
      }
    }
    
    FrequencyGraph(cr, active, x + 10, y + 22, 225, 95, freqState );
    
    TitleBar(cr, x,y, xS, yS, "Parametric EQ", active);
    
    if ( selected )
    {
      cr->rectangle(x, y, xS, yS);
      setColour( cr, COLOUR_ORANGE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
  }
}
