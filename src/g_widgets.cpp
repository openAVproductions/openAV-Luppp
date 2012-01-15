
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

#ifndef LUPPP_DRAWHELPER
#define LUPPP_DRAWHELPER

#include <gtkmm/drawingarea.h>

#include "g_widgets.hpp"

namespace Luppp
{
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c, float alpha)
  {
    switch( c )
    {
      case COLOUR_ORANGE_1:
        cr->set_source_rgba( 255 / 255.f, 104 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_ORANGE_2:
        cr->set_source_rgba( 178 / 255.f,  71 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_ORANGE_3:
        cr->set_source_rgba(  89 / 255.f,  35 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_GREEN_1:
        cr->set_source_rgba(  25 / 255.f, 255 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_GREEN_2:
        cr->set_source_rgba(  17 / 255.f, 179 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_GREEN_3:
        cr->set_source_rgba(   8 / 255.f,  89 / 255.f ,   0 / 255.f , alpha ); break;
      case COLOUR_BLUE_1:
        cr->set_source_rgba(   0 / 255.f, 153 / 255.f , 255 / 255.f , alpha ); break;
      case COLOUR_BLUE_2:
        cr->set_source_rgba(  20 / 255.f,  73 / 255.f , 109 / 255.f , alpha ); break;
      case COLOUR_BLUE_3:
        cr->set_source_rgba(   0 / 255.f,  53 / 255.f ,  89 / 255.f , alpha ); break;
      case COLOUR_PURPLE_1:
        cr->set_source_rgba( 230 / 255.f,   0 / 255.f , 255 / 255.f , alpha ); break;
      case COLOUR_PURPLE_2:
        cr->set_source_rgba( 161 / 255.f,   0 / 255.f , 179 / 255.f , alpha ); break;
      case COLOUR_PURPLE_3:
        cr->set_source_rgba(  80 / 255.f,   0 / 255.f ,  89 / 255.f , alpha ); break;
      case COLOUR_GREY_1:
        cr->set_source_rgba( 130 / 255.f, 130 / 255.f , 130 / 255.f , alpha ); break;
      case COLOUR_GREY_2:
        cr->set_source_rgba(  98 / 255.f,  98 / 255.f ,  98 / 255.f , alpha ); break;
      case COLOUR_GREY_3:
        cr->set_source_rgba(  66 / 255.f,  66 / 255.f ,  66 / 255.f , alpha ); break;
      case COLOUR_GREY_4:
        cr->set_source_rgba(  28 / 255.f,  28 / 255.f ,  28 / 255.f , alpha ); break;
      case COLOUR_RECORD_RED:
        cr->set_source_rgba(  226 / 255.f, 0/255.f , 0/255.f, alpha ); break;
      case COLOUR_TRANSPARENT:
        cr->set_source_rgba(  0, 0, 0, 0.f ); break;
      case COLOUR_BACKGROUND: default:
        cr->set_source_rgba(  40 / 255.f,  40 / 255.f ,  40 / 255.f , alpha ); break;
    }
  }
  
  void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c)
  {
    setColour(cr, c, 1.f);
  }
}


#endif
