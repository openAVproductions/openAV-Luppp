
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

#ifndef LUPPP_GUI_WIDGETS
#define LUPPP_GUI_WIDGETS

#include <iostream>

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include "g_equalizerstate.hpp"
#include "clipselectorstate.hpp"

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
    // specials
    COLOUR_BACKGROUND,
    COLOUR_RECORD_RED,
    COLOUR_TRANSPARENT,
  };
  
  void Rec ( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Solo( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Mute( Cairo::RefPtr<Cairo::Context> cr, float x, float y, int id, bool active);
  void Dial( Cairo::RefPtr<Cairo::Context> cr, bool, float x, float y, float value, DialMode mode);
  void Fader(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float value, float rms, float& falloff);
  
  void TitleBar( Cairo::RefPtr<Cairo::Context> cr, float x, float y, float xS, float yS, std::string name, bool active);
  //void WaveDisplay(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float xSize, float ySize, WaveDisplayState* state);
  void FrequencyGraph( Cairo::RefPtr<Cairo::Context> cr, bool, float x, float y, float xS, float yS, EqualizerState state);
  
  // helper functions
  void Block(Cairo::RefPtr<Cairo::Context> cr, float x, float y, ClipState, std::string);
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c);
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c, float alpha);
}

#endif

