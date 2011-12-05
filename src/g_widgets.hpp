

#ifndef LUPPP_GUI_WIDGETS
#define LUPPP_GUI_WIDGETS

#include <iostream>

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>


namespace Luppp
{
  enum DialMode {
    DIAL_MODE_NORMAL,
    DIAL_MODE_SEND,
    DIAL_MODE_PAN,
  };
  
  enum Colour {
    COLOUR_ORANGE_1 = 0,
    COLOUR_ORANGE_2,
    COLOUR_ORANGE_3,
    COLOUR_GREEN_1,
    COLOUR_GREEN_2,
    COLOUR_GREEN_3,
    COLOUR_BLUE_1,
    COLOUR_BLUE_2,
    COLOUR_BLUE_3,
    COLOUR_PURPLE_1,
    COLOUR_PURPLE_2,
    COLOUR_PURPLE_3,
    COLOUR_GREY_1,
    COLOUR_GREY_2,
    COLOUR_GREY_3,
    COLOUR_GREY_4,
    COLOUR_BACKGROUND,
    COLOUR_TRANSPARENT,
  };
  
  enum BlockState {
    BLOCK_STATE_EMPTY,
    BLOCK_STATE_LOADED,
    BLOCK_STATE_PLAYING,
    BLOCK_STATE_QUEUED,
    BLOCK_STATE_RECORDING,
  };
  
  void Rec ( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Solo( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Mute( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Dial( Cairo::RefPtr<Cairo::Context> cr, bool, float x, float y, float value, DialMode mode);
  void Fader(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float value, float rms, float peak);
  
  /*
  void TitleBar( Cairo::RefPtr<Cairo::Context> cr, float x, float y, float xS, float yS, std::string name, bool active);
  void WaveDisplay(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float xSize, float ySize, WaveDisplayState* state);
  void FrequencyGraph( Cairo::RefPtr<Cairo::Context> cr, bool, float x, float y, float xS, float yS, FrequencyGraphState state);
  */
  
  // helper functions
  void Block(Cairo::RefPtr<Cairo::Context> cr, float x, float y, BlockState active);
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c);
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c, float alpha);
}

#endif

