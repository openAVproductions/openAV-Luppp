/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef AVTK_DIAL_H
#define AVTK_DIAL_H

#include <stdio.h>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Slider.H>

namespace Avtk
{

class Dial : public Fl_Slider
{
public:
	Dial(int _x, int _y, int _w, int _h, const char* _label=0):
		Fl_Slider(_x, _y, _w, _h)
	{
		copy_label(_label);
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		drawLabel = true;

		mouseClickedY = 0;
		mouseClicked = false;

		highlight = false;
	}

	bool highlight;
	bool pan_style;
	int x, y, w, h;

	float radius;
	float lineWidth;

	int mouseClickedY;
	bool mouseClicked;

	bool drawLabel;

	void draw()
	{
		if (damage() & FL_DAMAGE_ALL) {
			if ( drawLabel ) {
				draw_label();
			}

			// * 0.9 for line width to remain inside redraw area
			if ( w > h )
				radius = (h / 2.f)*0.9;
			else
				radius = (w / 2.f)*0.9;

			lineWidth = 1.4 + radius / 12.f;

			cairo_t *cr = Fl::cairo_cc();

			cairo_save( cr );

			cairo_rectangle( cr, x, y, w, h );
			cairo_set_source_rgba(cr, 1.1, 0.1, 0.1, 0 );
			cairo_fill(cr);

			cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

			cairo_set_line_width(cr, lineWidth-0.2);
			cairo_move_to( cr, x+w/2,y+h/2);
			cairo_line_to( cr, x+w/2,y+h/2);
			cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0 );
			cairo_stroke(cr);

			cairo_arc(cr, x+w/2,y+h/2, radius, 2.46, 0.75 );
			cairo_set_source_rgb(cr, 0.1, 0.1, 0.1 );
			cairo_stroke(cr);

			if(!pan_style) {
				float angle = 2.46 + ( 4.54 * value() );
				cairo_arc(cr, x+w/2,y+h/2, radius, 2.46, angle );
			} else {
				float angle = 2.46 + ( 4.54 * value() );
				float mid   = 2.46 + ( 4.54 * 0.5 );
				if(angle >= mid)
					cairo_arc(cr, x+w/2,y+h/2, radius, mid, angle);
				else
					cairo_arc_negative(cr, x+w/2,y+h/2, radius, mid, angle);
			}
			cairo_line_to(cr, x+w/2,y+h/2);
			cairo_set_line_width(cr, lineWidth);
			cairo_set_source_rgba(cr, 1.0, 0.48,   0, 0.8);
			cairo_stroke(cr);

			cairo_restore( cr );
		}
	}

	void resize(int X, int Y, int W, int H)
	{
		Fl_Slider::resize(X,Y,W,H);
		x = X;
		y = Y;
		w = W;
		h = H;
		redraw();
	}

	int handle(int event)
	{
		//cout << "handle event type = " << event << " value = " << value() << endl;

		//Fl_Slider::handle( event );

		switch(event) {
		case FL_PUSH:
			highlight = 1;
			redraw();
			return 1;
		case FL_DRAG: {
			if ( Fl::event_state(FL_BUTTON1) ) {
				if ( mouseClicked == false ) { // catch the "click" event
					mouseClickedY = Fl::event_y();
					mouseClicked = true;
				}

				float deltaY = mouseClickedY - Fl::event_y();

				float val = value();
				val += deltaY / 100.f;

				if ( val > 1.0 ) val = 1.0;
				if ( val < 0.0 ) val = 0.0;

				set_value( val );

				mouseClickedY = Fl::event_y();
				redraw();
				do_callback(); // makes FLTK call "extra" code entered in FLUID
			}
		}
		return 1;
		case FL_MOUSEWHEEL: {
			float val = value() - Fl::event_dy() * 0.1;
			//printf("dial scroll event: %f\n", val);
			if ( val > 1.0 ) val = 1.0;
			if ( val < 0.0 ) val = 0.0;
			set_value( val );
			redraw();
			do_callback();
			return 1;
		}
		break;
		case FL_RELEASE:
			if (highlight) {
				highlight = 0;
				redraw();
				// never do anything after a callback, as the callback
				// may delete the widget!
			}
			mouseClicked = false;
			return 1;
		default:
			return Fl_Widget::handle(event);
		}
	}
};

} // Avtk

#endif // AVTK_DIAL_H
